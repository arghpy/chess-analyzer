#include "fen.h"
#include "raylib.h"
#include "init.h"
#include "render.h"
#include "core.h"
#include "rules/pieces.h"
#include <stdbool.h>

int main(void)
{
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 600, "Chess analyzer");
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
      if (IsKeyPressed(KEY_SPACE)) {
        chess_board.board_flipped = !chess_board.board_flipped;
        flip_board();
      }
      if(IsKeyPressed(KEY_P)) iterate_fen_positions();
      BeginDrawing();
      {
        ClearBackground(background_color);
        draw_chess_board(&font);
        if (chess_board.result == NONE) {
          if (chess_board.state.promote) promote_pawn(chess_board.moving.c_src, chess_board.moving.c_dest);
          else {
            draw_moving_piece();
          }
        } else draw_result(&font);

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
