#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include "types.h"
#include <stdbool.h>

extern Sound game_sounds[SOUNDS_COUNT];

bool load_sounds(void);
void unload_sounds(void);

#endif
