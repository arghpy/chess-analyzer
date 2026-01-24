#include "move.h"
#include "board.h"
#include "raylib.h"
#include <stdio.h>
#include "rules/pieces.h"
#include "rules/general.h"

bool is_legal_move(void)
{
  bool legal_move = true;

  // Check if legal move
  switch (chess_board.src_copy.piece.type) {
    case PAWN:
      break;
    case BISHOP:
      legal_move = bishop_is_legal_move();
      break;
    case KING:
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

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.dest = &chess_board.squares[y][x];
      if (CheckCollisionPointCircle(GetMousePosition(), chess_board.dest->CenterProximity.center, chess_board.dest->CenterProximity.r)) {
        if (
            (correct_color_turn(&chess_board.src_copy)) &&
            (! capture_ally(chess_board.dest, &chess_board.src_copy)) &&
            is_legal_move()
            ) {
          chess_board.squares[y][x].piece = chess_board.src_copy.piece;
          chess_board.piece_placed = true;
          change_chess_board_turn();
          break;
        }
      }
    }
    if (chess_board.piece_placed) break;
  }
  if (!chess_board.piece_placed)
    chess_board.src->piece = chess_board.src_copy.piece;
  chess_board.piece_placed = false; // Reset
}

void draw_drag_and_place(void)
{
  if (!chess_board.dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        chess_board.src_copy = chess_board.squares[y][x];
        chess_board.src      = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), chess_board.src_copy.rect) &&
            (chess_board.src_copy.piece.type != NO_PIECE)
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
    ChessSquare square = chess_board.src_copy;
    square.rect.x      = mouse_pos.x - chess_board.src_copy.rect.width / 2;
    square.rect.y      = mouse_pos.y - chess_board.src_copy.rect.height / 2;
    draw_piece(&square);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.dragging_piece = false;
      place_piece();
    }
  }
}

void check_hovering(void)
{
  ChessSquare *square = NULL;
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) &&
          (square->piece.type != NO_PIECE)) {
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
      chess_board.hovering_piece || chess_board.dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
}

void draw_moving_piece(void)
{
  check_hovering();
  draw_drag_and_place();
}
