#include "engine.h"

dynArray* collideArray;

object* AABBCollisionObj(entity** a);
void testCollision();
entity** getEntityByID(int ID);

void stub(){}
void collision_stub(entity** a, entity** b, float c){UNUSED(a);UNUSED(b);UNUSED(c);}
void processDeletes();
void deleteEntityInt(entity** entity);

/**
 * @brief      Creates an entity.
 *
 * @param[in]  objName         The name of the entity
 * @param[in]  rect            The rect describing the box of the object
 * @param[in]  xOffset         The X-offset (not supported)
 * @param[in]  yOffset         The Y-offset (not supported)
 * @param[in]  scale           The scale of the object
 * @param[in]  angle           The angle (rotated anticlockwise from up)
 * @param      texture         The texture (from getTexture())
 * @param[in]  collide         Collider flag
 * @param[in]  entity_handler  The entity handler pointer
 * @param      data            The data pointer
 * @param[in]  dataSize        The size of the data
 *
 * @return     Returns a new unique entity ID
 */
int createEntity(object obj, int collide, void (*entity_handler)(entity**), void* data, int dataSize, void (*collide_handler)(entity**, entity**, float), body* bodyData) {
	object* intObject = createObject(obj.name, obj.rect, obj.xOffset, obj.yOffset, obj.scale, obj.angle, obj.texture, obj.layer);

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
		.collide_handler = collide_handler == NULL ? *collision_stub : collide_handler,
		.collide = collide,
		.deleted = 0,
		.id = entityUID,
		.data = gmalloc(dataSize),
		.body = bodyData == NULL ? NULL : gmalloc(sizeof(body))
	};

	if (bodyData != NULL) {
		memmove((*intEntity)->body, bodyData, sizeof(body));
	}

	if (data != NULL) {
		memmove((*intEntity)->data, data, dataSize);
	} else {
		gfree((*intEntity)->data);
		(*intEntity)->data = NULL;
	}

	if (bodyData != NULL) {
		if ((*intEntity)->body->collision_type == BODY_STATIC) {
			(*intEntity)->body->body = cpSpaceAddBody(space, cpBodyNewKinematic());
		} else {
			float MOI = (intObject->rect.w * pow(intObject->rect.h, 3))/12.;
			(*intEntity)->body->body = cpSpaceAddBody(space, cpBodyNew((*intEntity)->body->mass, MOI)); //makes a dynamic body for u
		}

		//formula for MOI of a rectangle:
		/*
		I = w*h^3
			-----
			  12

		*/
		cpBodySetVelocity((*intEntity)->body->body, cpv((*intEntity)->body->velocity.x, (*intEntity)->body->velocity.y));
		cpBodySetPosition((*intEntity)->body->body, cpv(intObject->rect.x, intObject->rect.y));
		cpBodySetUserData((*intEntity)->body->body, intEntity);
		(*intEntity)->body->shape = cpSpaceAddShape(space, cpBoxShapeNew((*intEntity)->body->body, intObject->rect.w, intObject->rect.h, 0.0));
	}



	char buffer[18];
	itoa(entityUID, buffer);
	addToDictionary(entities, buffer, intEntity);
	entityCount++;

	return entityUID++;
}

/**
 * @brief      Gets the entity by ID.
 *
 * @param[in]  ID of entity
 *
 * @return     The entity by ID.
 */
entity** getEntityByID(int ID) {
	char buffer[18];
	itoa(ID, buffer);
	size_t entityValueIndex = findKey(entities, buffer);
	return entityValueIndex == NOVALUE ? NULL : *(entity***)getElement(entities->value, entityValueIndex);
}

/**
 * @brief      Traverses the entities and executes them
 */
void runEntities() {
	processPhysics();
	for (size_t i = 0; i < entities->key->arraySize; i++) {

		entity** internalEntity = *(entity***)getElement(entities->value, i);
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

		(*internalEntity)->entity_handler(internalEntity);
		//printDictionary(entities);
	}
	deleteEntities();

}


/**
 * @brief      Deletes an entity
 *
 * @param      intEntity  The entity pointer
 */
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

/**
 * @brief      Deletes all the entities
 */
void cleanEntities() {
	for (size_t i = 0; i < entities->key->arraySize; i++) {
		//deleteEntity(*(entity***)getElement(entities->value, i));
	}
	//deleteEntities();
	//freeDictionary(entities);
	return;
}

/**
 * @brief    Do not use this function! To be used internally by the entity system to delete entities
 *
 */
void deleteEntities() {
	if (deletedCount == 0) {
		return;
	}
	//logtofile("deleting entities", INF, "entities");
	for (size_t i = 0; i < entities->key->arraySize; i++) {
		entity* internalEntity = (**(entity***)getElement(entities->value, i));

		if (internalEntity == NULL || internalEntity->deleted != 1) {
		    continue;
		}
		//

		entityCount--;

		if (internalEntity->data != NULL) {
			gfree(internalEntity->data);
		}

		char buffer[18];
		itoa(internalEntity->object->id, buffer);
		removeObject(buffer);
		itoa(internalEntity->id, buffer);

		gfree(internalEntity);
		//gfree(entityIterator->value);
		//((entity**)entityIterator->value) = &0x0;
		removeKey(entities, buffer);



	}

	deletedCount = 0;
}

int entityQSort(const void* a, const void* b) {
	entity** entityA = *((entity***)a);
	entity** entityB = *((entity***)b);
	if ((*entityA)->object->rect.x < (*entityB)->object->rect.x) return -1;
	if ((*entityA)->object->rect.x > (*entityB)->object->rect.x) return 1;
	return 0;
}

float collisionFunction(entity** a, entity** b) {
	int entityCollider = (*a)->collide;
	int intEntityCollider = (*b)->collide;

	float status = 0.;

	if (entityCollider == COLLIDE_BOX && intEntityCollider == COLLIDE_BOX) {
		status = AABBCollision(a, b);
	} else if ((entityCollider == COLLIDE_BOX && intEntityCollider == COLLIDE_CIRCLE) ||
			   (entityCollider == COLLIDE_CIRCLE && intEntityCollider == COLLIDE_BOX)) {
		status = circleBoxCollision(a, b);
	} else if (entityCollider == COLLIDE_CIRCLE && intEntityCollider == COLLIDE_CIRCLE) {
		status = circleCircleCollision(a, b);
	}

	return status;
}

//TODO: make this an FP system, autogenerate the collider pairs at runtime
//also, return a collider object

/**
 * @brief      Tests collision
 */
void testCollision() {

	clearArray(collideArray);

	//sweep and prune time
	//this causes an uninitialied value call. weird.
	entity*** list = gmalloc(sizeof(entity**) * entities->key->arraySize);
	for (size_t i = 0; i < entities->key->arraySize; i++) {
		list[i] = *(entity***)getElement(entities->value, i);
	}
	//sort list by x value

	qsort(list, entities->key->arraySize, sizeof(entity**), entityQSort);

	dynArray* activeIntervals = createDynArray(sizeof(entity**));
	appendElement(activeIntervals, list[0]);

	//printf("\n\n\n");
	for (size_t i = 0; i < entities->key->arraySize; i++) {
		entity** entityB = list[i];
		if ((*entityB)->collide == COLLIDE_NONE) {
			continue;
		}

		//lets do s&p properly now
		x:
		for (size_t j = 0; j < activeIntervals->arraySize; j++) {
			entity** entityA = getElement(activeIntervals, j);
			if ((*entityB)->object->rect.x > (*entityA)->object->rect.w + (*entityA)->object->rect.x) {
				removeElement(activeIntervals,j);
				goto x;
			}
		}

		if (activeIntervals->arraySize != 0) {
			for (size_t j = 0; j < activeIntervals->arraySize; j++) {
				//printf("%ld\n", j);
				//printf("%ld\n", activeIntervals->arraySize);
				entity** entityA = getElement(activeIntervals, j);
				if (*entityA == *entityB) {
					continue;
				}
				double b1 = (*entityA)->object->rect.x;
				double b2 = (*entityA)->object->rect.x + (*entityA)->object->rect.w;
				double a1 = (*entityB)->object->rect.x;
				double a2 = (*entityB)->object->rect.x + (*entityB)->object->rect.w;

				//printf("%ld-%s: %f, %f\n %ld-%s: %f, %f\n\n", j, (*entityA)->object->name, a1, a2, i, (*entityB)->object->name, b1, b2);
				//test interval
				if ((a1 - b2) <= 0 && (b1 - a2) <= 0) {
					//collision succeeded!
					//printf("weewoo: %s\n", (*entityB)->object->name);
					float status = collisionFunction(entityA, entityB);
					//printf("%f\n", status);
					if (status > 0.) {
						appendElement(collideArray, &(collidePair){(*entityA), (*entityB)});
						(*entityA)->collide_handler(entityA, entityB, status);
						//printf("weewooA: %s\n", (*entityA)->object->name);
						(*entityB)->collide_handler(entityB, entityA, status);
						//printf("weewooB: %s\n", (*entityB)->object->name);
					}
				}
			}
		}
		appendElement(activeIntervals, entityB);
	}
	deleteArray(activeIntervals);
}

object* AABBCollisionObj(entity** a) {
	if ((*a) == NULL) {
		return NULL;
	}
	Rect rect1 = (*a)->object->rect;
	for (size_t i = 0; i < objects->key->arraySize; i++) {
		if (*(object**)getElement(objects->value, i) == (*a)->object || *(object**)getElement(objects->value, i) == NULL) {
			continue;
		}

		object intObject = **(object**)getElement(objects->value, i);
		Rect rect2 = intObject.rect;
		if (rect1.x < rect2.x + rect2.w &&
   		rect1.x + rect1.w > rect2.x &&
   		rect1.y < rect2.y + rect2.w &&
   		rect1.y + rect1.h > rect2.y) {
			//collision!
			return *(object**)getElement(objects->value, i);
   		}
	}
	return NULL;
}
