#include "fen.h"
#include "raylib.h"
#include "keyboard.h"
#include "init.h"
#include "render.h"
#include "rules/pieces.h"
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_FACTOR 70
#define WINDOW_WIDTH  (WINDOW_FACTOR * 16)
#define WINDOW_HEIGHT (WINDOW_FACTOR * 9)

int main(void)
{
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chess analyzer");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  char *font_path = "./assets/fonts/JetBrainsMono-Bold.ttf";
  Font font = LoadFont(font_path);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

  bool init = true;

  { // Initializing
    if (!load_starting_position()) init = false;
    if (!load_chess_pieces())      init = false;
    load_pawn_promotions();
  }

  if (init) {
    while(!WindowShouldClose()) {
      process_keyboard_events();
      fflush(stdout);
      BeginDrawing();
      {
        ClearBackground(background_color);
        draw_chess_board(&font);
        if (chess_board.result != NONE) draw_result(&font);
        else {
          if (chess_board.state.promote) promote_pawn(chess_board.promotion_square);
          else {
            draw_moving_piece();
          }
        }
        set_mouse_cursor();
      }
      EndDrawing();

      // Reset
      chess_board.state.piece_placed = false;
      chess_board.state.hovering_piece = false;
      chess_board.state.hovering_promotion = false;
      chess_board.state.captured = false;
      chess_board.moving.captured_piece = (ChessPiece){0};
      chess_board.moving.wrong_move = false;
      chess_board.state.promotion_done = false;
    }
  }

  { // Closing everything
    ut_da_free(&positions);
    unload_chess_pieces();
    UnloadFont(font);
    CloseWindow();
  }
  return 0;
}
