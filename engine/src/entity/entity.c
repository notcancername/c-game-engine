#include "engine.h"

void stub(){}

void processDeletes();
void deleteEntityInt(entity** entity);

int createEntity(const char* objName, Rect rect, int xOffset, int yOffset, float scale, double angle, int_Texture* texture, int collide, void (*entity_handler)(entity**), void* data, int dataSize) {
	object* intObject = createObject(objName, rect, xOffset, yOffset, scale, angle, texture);
	
	entity** intEntity;
	intEntity = gmalloc(sizeof(entity*));
	(*intEntity) = gmalloc(sizeof(entity));
	
	if (intObject == NULL) {
		logtofile("Object creation error, entity creation cannot continue!", ERR, "Entity");
		crash();
		return 0;
	}

	if (data == NULL) {
		dataSize = 0;
	}


	**intEntity = (entity) {
		.object = intObject,
		.entity_handler = entity_handler == NULL ? *stub : entity_handler,
		.collide = collide,
		.deleted = 0,
		.id = entityUID,
		.data = gmalloc(dataSize)
	};
	memcpy((*intEntity)->data, data, dataSize);

	char buffer[18];
	itoa(entityUID, buffer);
	addToDictionary(entities, buffer, intEntity);
	entityCount++;
	
	return entityUID++;
}

entity** getEntityByID(int ID) {
	char buffer[18];
	itoa(ID, buffer);
	dictionary entityDictionary = findKey(entities, buffer);
	return entityDictionary == NULL ? NULL : (entity**)entityDictionary->value; 
}

void runEntities() {
	dictionary entityIterator = entities->next;
	for (int i = 0; i < entityCount; i++) {
		if (entityIterator == NULL) {
		    return;
		}

		entity** internalEntity = entityIterator->value;
		if (internalEntity == NULL) {
		    continue;
		}
		if ((*internalEntity) == NULL) {
			continue;
		}

		if ((*internalEntity)->deleted == 1) {
			//logtofile("deleted entity still exists!", WRN, "Entity");
			deleteEntity(internalEntity);
			continue;
		}

		(*internalEntity)->entity_handler((entity**)entityIterator->value);
		entityIterator = entityIterator->next;
	}
	deleteEntities();
}

void deleteEntity(entity** intEntity) {
	if (intEntity == NULL) {
		return;
	}
	if ((*intEntity) == NULL) {
		return;
	}
	(*intEntity)->deleted = 1;
	deletedCount++;
}

void cleanEntities() {
	dictionary entityIterator = entities->next;
	while (entityIterator != NULL) {
		deleteEntity((entity**)entityIterator->value);
		entityIterator = entityIterator->next;
	}
	deleteEntities();
	freeDictionary(entities);
	return;
}

void deleteEntities() {
	if (deletedCount == 0) {
		return;
	}
	//logtofile("deleting entities", INF, "entities");
	dictionary entityIterator = entities->next;

	while (entityIterator != NULL) {
		if (entityIterator == NULL) {
		    return;
		}
		entity* internalEntity = (*(entity**)entityIterator->value);

		if (internalEntity == NULL || internalEntity->deleted != 1) {
		    entityIterator = entityIterator->next;
		    continue;
		}
		*((entity**)entityIterator->value) = NULL;
		entityIterator = entityIterator->next;
		
		entityCount--;
		gfree(internalEntity->data);
		char buffer[18];
		itoa(internalEntity->object->id, buffer);
		removeObject(buffer);
		itoa(internalEntity->id, buffer);

		gfree(internalEntity);
		gfree(entityIterator->value);
		removeKey(entities, buffer);



	}

	deletedCount = 0;
}	

//TODO: make this an FP system, autogenerate the collider pairs at runtime
//also, return a collider object

int testCollision(entity** a) {
	for (int i = 0; i < COLLIDE_SIZE; i++) {
		collideArray[i] = NULL;
	}

	int collisionCount = 0;

	int entityCollider = (*a)->collide;
	dictionary entityIterator = entities->next;
	while (entityIterator != NULL) {
		entity** intEntity = (entity**)entityIterator->value;
		if (intEntity == NULL) {
			entityIterator = entityIterator->next;
			continue;
		}

		if ((*intEntity) == NULL) {
			entityIterator = entityIterator->next;
			continue;
		}

		if (intEntity == a) {
			entityIterator = entityIterator->next;
			continue;
		}
		int intEntityCollider = (*intEntity)->collide;


		if (entityCollider == COLLIDE_NONE || intEntityCollider == COLLIDE_NONE) {
			entityIterator = entityIterator->next;
			continue;
		}

		int status = 0;

		if (entityCollider == COLLIDE_BOX && intEntityCollider == COLLIDE_BOX) {
			status = AABBCollision(a, intEntity);
		} else if ((entityCollider == COLLIDE_BOX && intEntityCollider == COLLIDE_CIRCLE) ||
				   (entityCollider == COLLIDE_CIRCLE && intEntityCollider == COLLIDE_BOX)) {
			status = circleBoxCollision(a, intEntity);
		} else if (entityCollider == COLLIDE_CIRCLE && intEntityCollider == COLLIDE_CIRCLE) {
			status = circleCircleCollision(a, intEntity);
		}

		if (status == 1 && collisionCount < COLLIDE_SIZE) {
			collideArray[collisionCount++] = intEntity;
		}
		entityIterator = entityIterator->next;
	}
	return collisionCount;
}



int circleCircleCollision(entity** a, entity** intEntity) {
	Rect entityRect = (*a)->object->rect;

	float radius1 = (entityRect.w*entityRect.h)/4;
	vec circleCenter = VECCNT(entityRect.x+entityRect.w, entityRect.y+entityRect.h);

	Rect intRect = (*intEntity)->object->rect;
	float radius2 = (intRect.w*intRect.h)/4;

	vec distance = vecSub(circleCenter, VECCNT(intRect.x+intRect.w, intRect.y+intRect.h));
	if (vecLength(distance) <= (radius1 + radius2)) {
		return 1;
	}
	return 0;
}

int circleBoxCollision(entity** a, entity** intEntity) {
	//search box on this, finding a circle collision
	//radius is the iterated entities' width, divided by 2
	//center is at the center of the object
	//god i fucking hate collision detection
	Rect entityRect = (*a)->object->rect;
		//impl from http://www.jeffreythompson.org/collision-detection/circle-rect.php
	Rect intRect = (*intEntity)->object->rect;
	float circleX = intRect.x + intRect.w/2;
	float circleY = intRect.y + intRect.h/2;
	float testX = circleX;
	float testY = circleY;
	float radius = (intRect.w+intRect.h)/2;

	if (circleX < entityRect.x) { 				      testX = entityRect.x;}        // left edge
	else if (circleX > entityRect.x + entityRect.w) { testX = entityRect.x + entityRect.w;}     // right edge

	if (circleY < entityRect.y) {        		      testY = entityRect.y;}       // top edge
	else if (circleY > entityRect.y + entityRect.h) { testY = entityRect.y + entityRect.h;}     // bottom edge

	vec distance = VECCNT(circleX - testX, circleY - testY);
	if (vecLength(distance) <= radius) {
		return 1;
	}

	return 0;
}

int AABBCollision(entity** a, entity** intEntity) {
	Rect rect1 = (*a)->object->rect;

	Rect rect2 = (*intEntity)->object->rect;
	if (rect1.x < rect2.x + rect2.w &&
   	rect1.x + rect1.w > rect2.x &&
   	rect1.y < rect2.y + rect2.w &&
   	rect1.y + rect1.h > rect2.y) {
			//collision!
		return 1;
   	}
		
	return 0;
}

object* AABBCollisionObj(entity** a) {
	if ((*a) == NULL) {
		return NULL;
	}
	Rect rect1 = (*a)->object->rect;
	dictionary objectDict = objects;
	for (int i = 0; i < objectUID; i++) {
		objectDict = objectDict->next;
		if (objectDict == NULL) {
			break;
		}
		if (objectDict->value == (*a)->object || objectDict->value == NULL) {
			continue;
		}

		object intObject = *(object*)objectDict->value;
		Rect rect2 = intObject.rect;
		if (rect1.x < rect2.x + rect2.w &&
   		rect1.x + rect1.w > rect2.x &&
   		rect1.y < rect2.y + rect2.w &&
   		rect1.y + rect1.h > rect2.y) {
			//collision!
			return (object*)&objectDict->value;
   		}
	}	
	return NULL;
}