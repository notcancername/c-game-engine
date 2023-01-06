#pragma once
#define RENDER_TEXTURE_H

typedef struct {
	SDL_Surface* surface;
	int textureID;
	int x;
	int y;
} int_Texture;

extern int textureCount;

extern const char** textureLocations;

extern dictionary textures;

void printTextures();
int loadTexture(const char *textureDir, const char* textureName);
void cleanTexture();
int_Texture* getTexture(const char* key);
int_Texture* colourTexture(RGBA rgba);
