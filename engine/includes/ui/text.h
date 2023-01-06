#pragma once
#define UI_TEXT_H

void initText();

extern FT_Face abyssinica;

extern dynArray* font;

typedef struct {
	uint txID;
	vec size;
	vec bearing;
	uint advance;
} character;

extern program textShader;

extern stack* textStack;

void renderText();
void drawText(char* text, float x, float y, float scale, RGBA colour);

typedef struct {
	char* text;
	float x;
	float y;
	float scale;
	RGBA colour;
} textStackEntry;
