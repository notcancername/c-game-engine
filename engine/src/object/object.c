#include "engine.h"

int objectUID;
int renderObjectSize[MAX_RENDER_LAYERS];
int objectCount;

/**
 * @brief      Creates an object.
 *
 * @param[in]  objName  The object name
 * @param[in]  rect     The rectangle describing the box of the object
 * @param[in]  xOffset  The X-offset (not in use)
 * @param[in]  yOffset  The y-offset (not in use)
 * @param[in]  scale    The scale of the object
 * @param[in]  angle    The angle (anticlockwise from up)
 * @param      tx       The texture (passed in from getTexture())
 *
 * @return     the object* pointing to the object
 */
object* createObject(const char* objName, Rect rect, int xOffset, int yOffset, float scale, double angle, int_Texture* tx, int layer) {

	object* intObject;
	intObject = gmalloc(sizeof(*intObject));

	if (layer >= MAX_RENDER_LAYERS) {
		layer = MAX_RENDER_LAYERS - 1;
	}

	vec objectVertexID = VECCNT(renderObjectSize[layer], layer);
//object vertex IDs need to be 2d, not 1d

	if (layer >= MAX_RENDER_LAYERS) {
		layer = MAX_RENDER_LAYERS - 1;
	}
	if (vertexPoolSize[layer] >= 1) {
		//printf("using vertex pool ID:%d!\n", vertexPool[vertexPoolSize-1]);
		objectVertexID = VECCNT(vertexPools[layer][vertexPoolSize[layer] - 1], layer);
		vertexPoolSize[layer]--;
	} else {
		vertices[layer] = grealloc(vertices[layer], sizeof(**vertices) * 16 * (renderObjectSize[layer]+1));

		elements[layer] = grealloc(elements[layer], sizeof(**elements) * 6 * (renderObjectSize[layer]+1));

		for (int i = 0; i < 16; i++) {
			vertices[layer][renderObjectSize[layer] * 16 + i] = 0.0f;
		}

		for (int i = 0; i < 6; i++) {
			elements[layer][renderObjectSize[layer] * 6 + i] = 0;
		}
		renderObjectSize[layer]++;
	}




	/*if (vertexPoolSize > FRAGMENTATION_LIMIT) {
		dictionary intObjDict = findTail(objects);
		for (int i = 0; i < FRAGMENTATION_LIMIT; i++) {
			object* intObject = (object*)intObjDict->value;
			int storedID = intObject->vertexID;
			intObject->vertexID = vertexPool[vertexPoolSize - 1 - i];
			vertexPool[vertexPoolSize-1-i] = storedID;
			dictionary intObjDict = findPrevKey(objects, intObjDict->key);
		}
	}*/

	*intObject = (object) {
        .rect    = rect,
        .xOffset = xOffset,
        .yOffset = yOffset,
        .scale   = scale,
        .angle   = angle,
        .id      = objectUID,
        .texture = tx == NULL ? getTexture("DEFAULT") : tx,
        .name    = objName == NULL ? strdup("NameProvidedWasNULL") : strdup(objName),
        .vertexID = objectVertexID,
        .layer = layer
    };

    updateObject(intObject);

    char buffer[18];
    itoa(intObject->id, buffer);
	addToDictionary(objects, buffer, intObject);
	objectCount++;
	objectUID++;

	return intObject;
}

/*object* getObjectID(int ID) {
	char buffer[18];
	itoa(ID, buffer);

	dictionary current = objects->next;
	while (current != NULL && current->key != NULL) {
		//printf("key: %s\n", current->key);
		if (strcmp(current->key, buffer) == 0) {
			return (object*)current->value;
		}
		current = current->next;
	}

	return NULL;
}*/

/**
 * @brief      Gets the object.
 *
 * @param[in]  key   The name of the object
 *
 * @return     The object.
 */
object* getObject(const char* key) {
	size_t intDictIndex = findKey(objects, key);
	return *(object**)getElement(objects->value, intDictIndex) == NULL ? NULL : *(object**)getElement(objects->value, intDictIndex);
}

/**
 * @brief      Updates the object vertices
 *
 * @param      intObject  The object*
 */
void updateObject(object* intObject) {
	if (intObject == NULL) {
		return;
	}

	float textureX = intObject->texture->x;
	float textureY = intObject->texture->y;
	float textureW = intObject->texture->surface->w;
	float textureH = intObject->texture->surface->h;
	float atlasH = textureAtlas->h;
	float atlasW = textureAtlas->w;


	Rect intRect = intObject->rect;
	intRect.x += intObject->xOffset;
	intRect.y += intObject->yOffset;
	int layer = intObject->layer;
	int count = intObject->vertexID.x;

	elements[layer][count * 6 + 0] = count * 4 + 0;
	elements[layer][count * 6 + 1] = count * 4 + 1;
	elements[layer][count * 6 + 2] = count * 4 + 2;
	elements[layer][count * 6 + 3] = count * 4 + 0;
	elements[layer][count * 6 + 4] = count * 4 + 3;
	elements[layer][count * 6 + 5] = count * 4 + 2;


	vertices[layer][count * 16 + 2] = (textureX + 0.5) / atlasW; // top left texcoord
	vertices[layer][count * 16 + 3] = (textureY - 0.5 + textureH) /atlasH;

	vertices[layer][count * 16 + 6] = (textureX - 0.5 + textureW) / atlasW; // top right texcoord
	vertices[layer][count * 16 + 7] = (textureY - 0.5 + textureH) / atlasH;

	vertices[layer][count * 16 + 10] = (textureX - 0.5 + textureW) / atlasW; // bottom right texcoord
	vertices[layer][count * 16 + 11] = (textureY + 0.5) / atlasH;

	vertices[layer][count * 16 + 14] = (textureX + 0.5) / atlasW; // bottom left texcoord
	vertices[layer][count * 16 + 15] = (textureY + 0.5) / atlasH;

	if (intObject->angle != 0) {
		double angle = intObject->angle;
		//find point of rotation
		vec rotationOrigin = VECCNT(intObject->rect.x + intObject->rect.w/2, intObject->rect.y + intObject->rect.h/2); //i think theres an off by 0.5 bug here
		vec rotation1 = vecRotateAroundOrigin(VECCNT(intRect.x, intRect.y), rotationOrigin, angle);
		vec rotation2 = vecRotateAroundOrigin(VECCNT(intRect.x + intRect.w, intRect.y), rotationOrigin, angle);
		vec rotation3 = vecRotateAroundOrigin(VECCNT(intRect.x + intRect.w, intRect.y + intRect.h), rotationOrigin, angle);
		vec rotation4 = vecRotateAroundOrigin(VECCNT(intRect.x, intRect.y + intRect.h), rotationOrigin, angle);

		vertices[layer][count * 16 + 0] = rotation1.x * 2.0 / SCREEN_WIDTH - 1.0;
		vertices[layer][count * 16 + 1] = rotation1.y * 2.0 / SCREEN_HEIGHT - 1.0;

		vertices[layer][count * 16 + 4] = rotation2.x * 2.0 / SCREEN_WIDTH - 1.0;
		vertices[layer][count * 16 + 5] = rotation2.y * 2.0 / SCREEN_HEIGHT - 1.0;

		vertices[layer][count * 16 + 8] = rotation3.x * 2.0 / SCREEN_WIDTH - 1.0;
		vertices[layer][count * 16 + 9] = rotation3.y * 2.0 / SCREEN_HEIGHT - 1.0;

		vertices[layer][count * 16 + 12] = rotation4.x * 2.0 / SCREEN_WIDTH - 1.0;
		vertices[layer][count * 16 + 13] = rotation4.y * 2.0 / SCREEN_HEIGHT - 1.0;

		//(object - object center) rotated by angle
		//angle rotated + object center = rotation
		return;
	}
	vertices[layer][count * 16 + 0] = intRect.x * 2.0 / SCREEN_WIDTH - 1.0;  // top left
	vertices[layer][count * 16 + 1] = intRect.y * 2.0 / SCREEN_HEIGHT - 1.0;

	vertices[layer][count * 16 + 4] = (intRect.x + intRect.w) * 2.0 / SCREEN_WIDTH - 1.0; //top right
	vertices[layer][count * 16 + 5] = intRect.y * 2.0 / SCREEN_HEIGHT - 1.0;

	vertices[layer][count * 16 + 8] = (intRect.x + intRect.w) * 2.0 / SCREEN_WIDTH - 1.0; //bottom right
	vertices[layer][count * 16 + 9] = (intRect.y + intRect.h) * 2.0 / SCREEN_HEIGHT - 1.0;

	vertices[layer][count * 16 + 12] = intRect.x * 2.0 / SCREEN_WIDTH - 1.0; //bottom left
	vertices[layer][count * 16 + 13] = (intRect.y + intRect.h) * 2.0 / SCREEN_HEIGHT - 1.0;
}

/**
 * @brief      Removes an object.
 *
 * @param[in]  key   The name of the object
 */
void removeObject(const char* key) {
	size_t objectDictIndex = findKey(objects, key);
	if (objectDictIndex == NOVALUE) {
		char error[256];
		sprintf(error, "object \"%s\" could not be found, returning early", key);
		logtofile(error, WRN, "Object");
		return;
	}

	object* intObject = *(object**)getElement(objects->value, objectDictIndex);
	if (intObject == NULL) {
		logtofile("Unable to delete object, crashing!", ERR, "Entities");
		crash();
	}
	int layer = intObject->layer;

	for (int i = 0; i < 16; i++) {
		vertices[layer][(int)intObject->vertexID.x * 16 + i] = 0.0f;

	}
	for (int i = 0; i < 6; i++) {
		elements[layer][(int)intObject->vertexID.x * 6 + i] = 0;
	}

	vertexPools[layer] = grealloc(vertexPools[layer], sizeof(**vertexPools) * (vertexPoolSize[layer] + 2));
	vertexPools[layer][vertexPoolSize[layer]] = (int)intObject->vertexID.x;
	vertexPoolSize[layer]++;

	gfree((char*)intObject->name);
	gfree(*(object**)getElement(objects->value, objectDictIndex));
	removeKey(objects, key);

	objectCount--;
}
