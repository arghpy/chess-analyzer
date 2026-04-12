#include "core.h"
#include "protocols/fen.h"
#include "raylib.h"
#include "input.h"
#include "init.h"
#include "render.h"
#include "sound.h"
#include <stdbool.h>
#include <stdio.h>


int main(void)
{
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess analyzer");
  SetTargetFPS(60);

  char *font_path = "./assets/fonts/JetBrainsMono-Bold.ttf";
  Font general_font = LoadFontEx(font_path, SQUARE_SIZE * 0.3f, NULL, 0);
  Font big_font = LoadFontEx(font_path, SQUARE_SIZE * 0.8f, NULL, 0);

  bool init = true;

  InitAudioDevice(); // Initialize audio device and context
  if (!IsAudioDeviceReady()) {// Check if audio device has been initialized successfully
    fprintf(stderr, "Failed to initialize audio device.\n");
    init = false;
  }

  { // Initializing
    if (init && !load_starting_position()) init = false;
    if (init && !load_chess_pieces())      init = false;
    if (init && !load_sounds())            init = false;
    if (init) load_pawn_promotions();
  }

  if (init) {
    while(!WindowShouldClose()) {
      process_keyboard_events();
      BeginDrawing();
      {
        ClearBackground(background_color);
        draw_chess_board(&general_font);
        draw_san_window(&general_font);
        draw_copy_fen_button(&general_font);

        process_game_states(&big_font);

        set_mouse_cursor();
        if (chess_board.action_sound != NOTHING) PlaySound(game_sounds[chess_board.action_sound]);
        chess_board.action_sound = NOTHING;
      }
      EndDrawing();
    }
  }

  { // Closing everything
    if (IsAudioDeviceReady()) CloseAudioDevice(); // Close the audio device and context
    unload_sounds();
    ut_da_free(&positions);
    unload_chess_pieces();
    UnloadFont(general_font);
    UnloadFont(big_font);
    CloseWindow();
  }
  return 0;
}
