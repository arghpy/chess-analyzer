#include "core.h"
#include "fen.h"
#include "render.h"
#include "raylib.h"
#include "rules/pieces.h"
#include "rules/general.h"
#include <stddef.h>
#include <stdio.h>

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.moving.c_dest     = &chess_board.squares[y][x];
      chess_board.moving.dest_piece = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(),
                                    chess_board.moving.c_dest->center_proximity.center,
                                    chess_board.moving.c_dest->center_proximity.r)) {
        if (valid_move(chess_board.moving.c_src, chess_board.moving.c_dest)) {
          if (chess_board.moving.c_dest->piece.type != NO_PIECE) {
            chess_board.state.captured = true;
            chess_board.moving.captured_piece = chess_board.moving.c_dest->piece;
          }
          chess_board.moving.c_dest->piece = chess_board.moving.src_piece;
          chess_board.state.placed_piece = true;

          if (verify_check(chess_board.moving.src_piece.color)) {
            chess_board.moving.c_dest->piece = chess_board.moving.dest_piece;
            chess_board.state.placed_piece = false;
            break;
          } else chess_board.state.is_check = false;
          gave_check();

          // Reset original colors
          if (chess_board.moving.p_src != NULL) {
            ptrdiff_t p_s_index = chess_board.moving.p_src - &chess_board.squares[0][0];
            int ys = p_s_index / NS;
            int xs = p_s_index % NS;

            chess_board.moving.p_src->board_color = square_color[(xs + ys) % 2];
          }

          if (chess_board.moving.p_dest != NULL) {
            ptrdiff_t p_d_index = chess_board.moving.p_dest - &chess_board.squares[0][0];
            int yd = p_d_index / NS;
            int xd = p_d_index % NS;

            chess_board.moving.p_dest->board_color = square_color[(xd + yd) % 2];
          }
          chess_board.moving.p_src  = chess_board.moving.c_src;
          chess_board.moving.p_dest = chess_board.moving.c_dest;

          chess_board.moving.p_src->board_color  = color_occupied_square(chess_board.moving.p_src);
          chess_board.moving.p_dest->board_color = color_occupied_square(chess_board.moving.p_dest);

          // Check promotion
          if (chess_board.moving.c_dest->piece.type == PAWN) {
            ptrdiff_t d_index = chess_board.moving.c_dest - &chess_board.squares[0][0];
            int yd = d_index / NS;

            if (yd == 0 || yd == (NS - 1)) {
              chess_board.state.promote = true;
              promote_pawn(chess_board.moving.c_src, chess_board.moving.c_dest);
            }
          }

          change_chess_board_turn();
          break;
        }
      }
    }
    if (chess_board.state.placed_piece) break;
  }
  if (!chess_board.state.placed_piece)
    chess_board.moving.c_src->piece = chess_board.moving.src_piece;
}

void draw_drag_and_place(void)
{
  if (!chess_board.state.dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        chess_board.moving.src_piece = chess_board.squares[y][x].piece;
        chess_board.moving.c_src     = &chess_board.squares[y][x];
        ChessSquare square    = chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
            (square.piece.type != NO_PIECE)
            ) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            chess_board.state.dragging_piece = true;
            reset_chess_square(&chess_board.squares[y][x]);
            break;
          }
        }
      }
      if (chess_board.state.dragging_piece) break;
    }
  }

  if (chess_board.state.dragging_piece) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle square = chess_board.moving.c_src->rect;
    square.x      = mouse_pos.x - SQUARE_SIZE / 2;
    square.y      = mouse_pos.y - SQUARE_SIZE / 2;

    ChessSquare moving_piece = {0};
    moving_piece.piece = chess_board.moving.src_piece;
    moving_piece.rect = square;
    draw_piece(&moving_piece);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.state.dragging_piece = false;
      place_piece();
      verify_if_any_legal_move(chess_board.color_turn);
      if (chess_board.enpassant.allowed) {
        if (chess_board.enpassant.allowed_by_color == chess_board.color_turn) {
          chess_board.enpassant.allowed = false;
          chess_board.enpassant.square = NULL;
        }
      }

      // Record which color piece moved
      if (chess_board.state.placed_piece) {
        generate_fen_position();
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

        chess_board.result.draw = (chess_board.result.draw ? chess_board.result.draw : chess_board.halfmoves == 50);
      }
    }
  }
}

void check_pieces_hovering(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare square = chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
          (square.piece.type != NO_PIECE)) {
        chess_board.state.hovering_piece = true;
        break;
      }
    }
    if (chess_board.state.hovering_piece) break;
  }
}

void set_mouse_cursor(void)
{
  SetMouseCursor(
      chess_board.state.hovering_promotion ||
      chess_board.state.hovering_piece ||
      chess_board.state.dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
}

void draw_moving_piece(void)
{
  if (!chess_board.state.promote) {
    check_pieces_hovering();
    draw_drag_and_place();
  }
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}
