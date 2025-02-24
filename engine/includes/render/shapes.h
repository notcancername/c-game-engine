#pragma once
#define RENDER_SHAPES_H

enum ShapeType {
	SHAPE_CIRCLE,
	SHAPE_LINE,
	SHAPE_END
};

typedef struct {
	enum ShapeType type;
	vec start;
	vec end;
	float radius;
	RGBA colour;
} shape;

extern stack* shapeStack;

extern program circleShader;
extern program lineShader;


void initShapes();

int renderShapes();
int drawCircle(vec pos, float radius, RGBA colour);
int drawLine(vec start, vec end, RGBA colour, float thickness);
