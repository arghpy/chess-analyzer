#include "core.h"
#include "render.h"
#include "raylib.h"
#include "rules/pieces.h"
#include "rules/general.h"
#include <stddef.h>
#include <stdio.h>

bool is_legal_move(ChessSquare *src, ChessSquare *dest, ChessPieceType type)
{
  bool legal_move = false;

  // Check if legal move
  switch (type) {
    case PAWN:
      legal_move = pawn_is_legal_move(src, dest);
      break;
    case BISHOP:
      legal_move = bishop_is_legal_move(src, dest);
      break;
    case KING:
      legal_move = king_is_legal_move(src, dest);
      break;
    case KNIGHT:
      legal_move = knight_is_legal_move(src, dest);
      break;
    case QUEEN:
      legal_move = queen_is_legal_move(src, dest);
      break;
    case ROOK:
      legal_move = rook_is_legal_move(src, dest);
      break;
    case NO_PIECE:
      break;
    case PIECE_COUNT:
      break;
    }
  return legal_move;
}

bool valid_move(ChessSquare *src, ChessSquare *dest)
{
  return (correct_color_turn() &&
      !capture_ally(dest) &&
      !capture_king(dest) &&
      !still_on_src_square(src, dest) &&
      is_legal_move(src, dest, chess_board.src_piece.type));
}

bool verify_check(ChessPieceColor verify_color)
{
  bool check = false;
  ChessSquare *ally_king = NULL;

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *s = &chess_board.squares[y][x];
      if (s->piece.color == verify_color && s->piece.type == KING)
        ally_king = s;
      if (ally_king != NULL) break;
    }
    if (ally_king != NULL) break;
  }

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *src = &chess_board.squares[y][x];
      if (src->piece.color != chess_board.color_turn) {
        check = is_legal_move(src, ally_king, src->piece.type);
      }
      if (check) break;
    }
    if (check) break;
  }
  return check;
}

bool verify_checkmate(ChessPieceColor verify_color)
{
  bool checkmate = true;

  // Iterate over all enemy pieces:
  // - try to make all the moves with them
  // - if they are legal and is still check, continue
  // - when no more enemy pieces are left to verify -> checkmate
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *enemy = &chess_board.squares[y][x];
      ChessPiece enemy_copy = chess_board.squares[y][x].piece;
      if (enemy->piece.color == verify_color) {
        for (int j = 0; j < NS; j++) {
          for (int i = 0; i < NS; i++) {
            ChessSquare *square = &chess_board.squares[j][i];
            ChessPiece square_copy = chess_board.squares[j][i].piece;

            if ((enemy->piece.color != square->piece.color) &&
                !capture_king(square) &&
                !still_on_src_square(enemy, square) &&
                is_legal_move(enemy, square, enemy->piece.type)) {
              // Add piece to square and empty enemy
              square->piece = enemy_copy;
              reset_chess_square(enemy);

              if (verify_check(square->piece.color)) {
                square->piece = square_copy;
                enemy->piece = enemy_copy;
              } else {
                square->piece = square_copy;
                enemy->piece = enemy_copy;
                checkmate = false;
                break;
              }
            }
          }
          if (!checkmate) break;
        }
      }
      if (!checkmate) break;
    }
    if (!checkmate) break;
  }
  chess_board.state.is_checkmate = checkmate;
  return checkmate;
}

void gave_check(void)
{
  ChessSquare *enemy_king = NULL;

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *s = &chess_board.squares[y][x];
      if (s->piece.color != chess_board.color_turn && s->piece.type == KING)
        enemy_king = s;
      if (enemy_king != NULL) break;
    }
    if (enemy_king != NULL) break;
  }

  if (is_legal_move(chess_board.c_dest, enemy_king, chess_board.c_dest->piece.type))
    chess_board.state.is_check = true;
}

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.c_dest     = &chess_board.squares[y][x];
      chess_board.dest_piece = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(),
                                    chess_board.c_dest->center_proximity.center,
                                    chess_board.c_dest->center_proximity.r)) {
        if (valid_move(chess_board.c_src, chess_board.c_dest)) {
          chess_board.c_dest->piece = chess_board.src_piece;
          chess_board.state.placed_piece = true;

          if (verify_check(chess_board.src_piece.color)) {
            chess_board.c_dest->piece = chess_board.dest_piece;
            chess_board.state.placed_piece = false;
            break;
          } else chess_board.state.is_check = false;
          gave_check();

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

          if (chess_board.c_dest->piece.type == PAWN) {
            ptrdiff_t d_index = chess_board.c_dest - &chess_board.squares[0][0];
            int yd = d_index / NS;

            if (yd == 0 || yd == (NS - 1)) {
              chess_board.state.promote = true;
              promote_pawn(chess_board.c_src, chess_board.c_dest);
            }
          }
          chess_board.p_src  = chess_board.c_src;
          chess_board.p_dest = chess_board.c_dest;

          chess_board.p_src->board_color  = color_occupied_square(chess_board.p_src);
          chess_board.p_dest->board_color = color_occupied_square(chess_board.p_dest);

          change_chess_board_turn();
          break;
        }
      }
    }
    if (chess_board.state.placed_piece) break;
  }
  if (!chess_board.state.placed_piece)
    chess_board.c_src->piece = chess_board.src_piece;
  chess_board.state.placed_piece = false; // Reset
}

void draw_drag_and_place(void)
{
  if (!chess_board.state.dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        chess_board.src_piece = chess_board.squares[y][x].piece;
        chess_board.c_src       = &chess_board.squares[y][x];
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
    Rectangle square = chess_board.c_src->rect;
    square.x      = mouse_pos.x - SQUARE_SIZE / 2;
    square.y      = mouse_pos.y - SQUARE_SIZE / 2;

    ChessSquare moving_piece = {0};
    moving_piece.piece = chess_board.src_piece;
    moving_piece.rect = square;
    draw_piece(&moving_piece);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.state.dragging_piece = false;
      place_piece();
      if (chess_board.state.is_check) verify_checkmate(chess_board.color_turn);
      if (chess_board.state.enpassant_allowed) {
        if (chess_board.state.enpassant_allowed_by->piece.color == chess_board.color_turn) {
          chess_board.state.enpassant_allowed = false;
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
