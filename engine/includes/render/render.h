#pragma once
#define RENDER_RENDER_H

extern int frameCount;

int render();

extern Rect viewport;

extern int renderedObjects;

extern float* renderVertices[MAX_RENDER_LAYERS];
extern int* renderElements[MAX_RENDER_LAYERS];
extern int renderCounts[MAX_RENDER_LAYERS];
