#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
  NOTHING,
  CAPTURE,
  CASTLE,
  GAME_END,
  GAME_START,
  ILLEGAL,
  MOVE,
  MOVE_CHECK,
  PROMOTE,
  SOUNDS_COUNT
} GameSound;

extern Sound game_sounds[SOUNDS_COUNT];

bool load_sounds(void);
void unload_sounds(void);

#endif
