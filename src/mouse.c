#include "mouse.h"
#include "board.h"
#include "raylib.h"
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
      Circle c      = chess_board.squares[y][x].CenterProximity;
      ChessPiece cp = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(), c.center, c.r)) {
        if (
            (get_piece_color(chess_board.src_copy.piece) == chess_board.color_turn) &&
            (get_piece_color(cp) != get_piece_color(chess_board.src_copy.piece))
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
        chess_board.src = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), chess_board.src_copy.rect) &&
            (chess_board.src_copy.piece != NO_PIECE)
            ) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            chess_board.dragging_piece = true;
            chess_board.squares[y][x].piece = NO_PIECE;
            break;
          }
        }
      }
      if (chess_board.dragging_piece) break;
    }
  }

  if (chess_board.dragging_piece) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle rect = {
      .x      = mouse_pos.x - chess_board.src_copy.rect.width / 2,
      .y      = mouse_pos.y - chess_board.src_copy.rect.height / 2,
      .width  = chess_board.src_copy.rect.width,
      .height = chess_board.src_copy.rect.height
    };
    ChessPiece type = chess_board.src_copy.piece;
    Texture2D *piece = &chess_pieces[type];
    draw_piece(piece, &rect);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.dragging_piece = false;
      place_piece();
    }
  }
}

void draw_mouse_cursor(void)
{
  ChessSquare *square = NULL;
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) && (square->piece != NO_PIECE)) {
        chess_board.hovering_piece = true;
        break;
      }
    }
    if (chess_board.hovering_piece) break;
  }

  draw_drag_and_place();

  SetMouseCursor(
      chess_board.hovering_piece || chess_board.dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
  chess_board.hovering_piece = false; // Reset
}
