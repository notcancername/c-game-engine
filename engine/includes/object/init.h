#pragma once
#define OBJECT_INIT_H

typedef struct {
	const char* name;
	int id;

	int xOffset;
	int yOffset;
	float scale;
	double angle;

	int_Texture* texture;
	Rect rect;

	vec vertexID;
	int layer;
} object;

extern dictionary objects;
extern int* vertexPools[MAX_RENDER_LAYERS];
extern int vertexPoolSize[MAX_RENDER_LAYERS];


void initObjects();
void cleanObjects();
extern program objectShader;
