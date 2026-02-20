#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "keyboard.h"
#include "init.h"
#include "render.h"
#include "rules/general.h"
#include "rules/pieces.h"
#include <stdbool.h>

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
      BeginDrawing();
      {
        ClearBackground(background_color);
        draw_chess_board(&font);
        if (chess_board.result != NONE) draw_result(&font);
        else {
          if (chess_board.state.promote) {
            draw_promotion_pieces(chess_board.promotion_square);
            select_for_promotion(chess_board.promotion_square);
          } else {
            if (!chess_board.state.promotion_done) {
              draw_moving_piece();
            } else {
              if (chess_board.enpassant.done) chess_board.enpassant.done = false;

              // Record which color piece moved
              if      (chess_board.moving.src_piece.color == W) chess_board.state.w_moved = true;
              else if (chess_board.moving.src_piece.color == B) chess_board.state.b_moved = true;

              // Full moves
              if (chess_board.state.w_moved && chess_board.state.b_moved) {
                chess_board.fullmoves += 1;
                chess_board.state.w_moved = false;
                chess_board.state.b_moved = false;
              }

              // Half moves
              if (chess_board.moving.src_piece.type == PAWN || chess_board.state.captured)
                chess_board.halfmoves = 0;
              else chess_board.halfmoves += 1;

              if (chess_board.halfmoves == 50) chess_board.result = DRAW;

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
      }
      EndDrawing();
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
