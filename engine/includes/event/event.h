#pragma once
#define EVENT_EVENT_H

void processEvents();

extern int eventCount;
extern SDL_Event events[256];

extern const Uint8* keyPresses;
