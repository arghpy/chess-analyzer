#include <stddef.h>
#include <stdio.h>
#include "sound.h"

#define MAX_SOUND_VOLUME 1.0f

Sound game_sounds[SOUNDS_COUNT] = {0};

void unload_sounds(void)
{
  for (size_t i = 1; i < SOUNDS_COUNT; i++) // 0 is Nothing
    UnloadSound(game_sounds[i]);
}

bool load_sounds(void)
{
  bool loaded = true;

  game_sounds[CAPTURE]    = LoadSound("./assets/sound/capture.wav");
  game_sounds[CASTLE]     = LoadSound("./assets/sound/castle.wav");
  game_sounds[GAME_END]   = LoadSound("./assets/sound/game-end.wav");
  game_sounds[GAME_START] = LoadSound("./assets/sound/game-start.wav");
  game_sounds[ILLEGAL]    = LoadSound("./assets/sound/illegal.wav");
  game_sounds[MOVE]       = LoadSound("./assets/sound/move.wav");
  game_sounds[MOVE_CHECK] = LoadSound("./assets/sound/move-check.wav");
  game_sounds[PROMOTE]    = LoadSound("./assets/sound/promote.wav");

  for (size_t i = 1; i < SOUNDS_COUNT; i++) { // 0 is Nothing
    if (!IsSoundValid(game_sounds[i])) {
      fprintf(stderr, "Sound not valid: %s:%zu.\n", __FILE__, __LINE__ - 3 - SOUNDS_COUNT + i);
      loaded = false;
      break;
    }
    SetSoundVolume(game_sounds[i], MAX_SOUND_VOLUME);
  }
  return loaded;
}
