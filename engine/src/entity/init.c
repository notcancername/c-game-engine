#include "engine.h"

int entityCount;
int entityUID;


dictionary entities;
int deletedCount;

/**
 * @brief      Initialises the entities
 */
void initEntities() {
	entities = createDictionary();
	collideArray = createDynArray(sizeof(collidePair));
	entityUID = 0;
	entityCount = 0;
	deletedCount = 0;
	initPhysics();
}
