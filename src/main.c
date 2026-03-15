#include "core.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "keyboard.h"
#include "init.h"
#include "render.h"
#include "rules/general.h"
#include "rules/pieces.h"
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
        draw_san(&general_font);
        if (chess_board.result != NONE) {
          draw_result(&big_font);
        } else {
          if (chess_board.state.promote) {
            draw_promotion_pieces(chess_board.promotion_square);
            select_for_promotion(chess_board.promotion_square);
          } else {
            if (!chess_board.state.promotion_done) {
              draw_moving_piece();
            } else {
              increment_game_states();
              generate_fen_position();
              verify_if_any_legal_move(chess_board.color_turn);
              generate_san();

              // Reset
              chess_board.state.promotion_done = false;
              chess_board.moving.c_src = NULL;
              chess_board.moving.c_dest = NULL;
              chess_board.castle.castled = NO;

              if (chess_board.state.captured) {
                chess_board.state.captured = false;
                chess_board.moving.captured_piece = (ChessPiece){0};
              }
            }
          }
        }
        set_mouse_cursor();
        if (chess_board.action_sound != NOTHING) PlaySound(game_sounds[chess_board.action_sound]);
        chess_board.action_sound = NOTHING;
      }
      EndDrawing();
      if (chess_board.state.w_moved && chess_board.state.b_moved) {
        chess_board.state.w_moved = false;
        chess_board.state.b_moved = false;
      }
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
