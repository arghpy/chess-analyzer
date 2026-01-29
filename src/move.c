#include "move.h"
#include "board.h"
#include "raylib.h"
#include "rules/pieces.h"
#include "rules/general.h"
#include <stddef.h>
#include <stdio.h>

bool is_legal_move(void)
{
  bool legal_move = true;

  // Check if legal move
  switch (chess_board.src_piece.type) {
    case PAWN:
      legal_move = pawn_is_legal_move();
      break;
    case BISHOP:
      legal_move = bishop_is_legal_move();
      break;
    case KING:
      legal_move = king_is_legal_move();
      break;
    case KNIGHT:
      legal_move = knight_is_legal_move();
      break;
    case QUEEN:
      legal_move = queen_is_legal_move();
      break;
    case ROOK:
      legal_move = rook_is_legal_move();
      break;
    case NO_PIECE:
      break;
    case PIECE_COUNT:
      break;
    }
  return legal_move;
}

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

Color ApplyTint(Color base, Color tint)
{
  Color out = {
    .r = (unsigned char)((base.r * tint.r) / 255),
    .g = (unsigned char)((base.g * tint.g) / 255),
    .b = (unsigned char)((base.b * tint.b) / 255),
    .a = base.a
  };
  return out;
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.c_dest = &chess_board.squares[y][x];
      if (CheckCollisionPointCircle(GetMousePosition(), chess_board.c_dest->center_proximity.center, chess_board.c_dest->center_proximity.r)) {
        if (correct_color_turn() &&
            !capture_ally() &&
            !capture_king() &&
            !still_on_src_square() && 
            is_legal_move()) {
          // Reset original colors
          if (chess_board.p_src != NULL) {
            ptrdiff_t p_s_index = chess_board.p_src - &chess_board.squares[0][0];
            int ys = p_s_index / NS;
            int xs = p_s_index % NS;

            chess_board.p_src->board_color = square_color[(xs + ys) % 2];
          }

          if (chess_board.p_dest != NULL) {
            ptrdiff_t p_d_index = chess_board.p_dest - &chess_board.squares[0][0];
            int yd = p_d_index / NS;
            int xd = p_d_index % NS;

            chess_board.p_dest->board_color = square_color[(xd + yd) % 2];
          }

          chess_board.squares[y][x].piece = chess_board.src_piece;
          chess_board.piece_placed = true;

          if (chess_board.c_dest->piece.type == PAWN) {
            ptrdiff_t d_index = chess_board.c_dest - &chess_board.squares[0][0];
            int yd = d_index / NS;

            if (yd == 0 || yd == (NS - 1)) {
              chess_board.promote = true;
              promote_pawn(chess_board.c_dest);
            }
          }
          chess_board.p_src  = chess_board.c_src;
          chess_board.p_dest = chess_board.c_dest;

          chess_board.p_src->board_color  = ColorIsEqual(chess_board.p_src->board_color, square_color[LIGHT_TILE]) ?
                                            (Color){0xF6, 0xEA, 0x72, 0xFF} : (Color){0xDD, 0xC3, 0x4C, 0xFF};
          chess_board.p_dest->board_color = ColorIsEqual(chess_board.p_dest->board_color, square_color[LIGHT_TILE]) ?
                                            (Color){0xF6, 0xEA, 0x72, 0xFF} : (Color){0xDD, 0xC3, 0x4C, 0xFF};

          change_chess_board_turn();
          break;
        }
      }
    }
    if (chess_board.piece_placed) break;
  }
  if (!chess_board.piece_placed)
    chess_board.c_src->piece = chess_board.src_piece;
  chess_board.piece_placed = false; // Reset
}

void draw_drag_and_place(void)
{
  if (!chess_board.dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        chess_board.src_piece = chess_board.squares[y][x].piece;
        chess_board.c_src       = &chess_board.squares[y][x];
        ChessSquare square    = chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
            (square.piece.type != NO_PIECE)
            ) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            chess_board.dragging_piece = true;
            reset_chess_square(&chess_board.squares[y][x]);
            break;
          }
        }
      }
      if (chess_board.dragging_piece) break;
    }
  }

  if (chess_board.dragging_piece) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle square = chess_board.c_src->rect;
    square.x      = mouse_pos.x - SQUARE_SIZE / 2;
    square.y      = mouse_pos.y - SQUARE_SIZE / 2;

    ChessSquare moving_piece = {0};
    moving_piece.piece = chess_board.src_piece;
    moving_piece.rect = square;
    draw_piece(&moving_piece);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.dragging_piece = false;
      place_piece();
      if (chess_board.enpassant_allowed) {
        if (chess_board.enpassant_allowed_by->piece.color == chess_board.color_turn) {
          chess_board.enpassant_allowed = false;
        }
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
        chess_board.hovering_piece = true;
        break;
      }
    }
    if (chess_board.hovering_piece) break;
  }
}

void set_mouse_cursor(void)
{
  SetMouseCursor(
      chess_board.hovering_promotion || chess_board.hovering_piece || chess_board.dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
}

void draw_moving_piece(void)
{
  if (!chess_board.promote) {
    check_pieces_hovering();
    draw_drag_and_place();
  }
}
