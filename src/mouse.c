#include "mouse.h"
#include "board.h"
#include "raylib.h"
#include <stdio.h>


bool hovering_piece = false;
bool dragging_piece = false;
bool piece_placed = false;

// Those need to be globals to persist between frames
ChessSquare *orig_dragged_piece = {0};
ChessSquare dragged_piece = {0};

void change_chess_board_turn(void)
{
  if (chess_board.turn == W) chess_board.turn = B;
  else chess_board.turn = W;
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      Circle c      = chess_board.squares[y][x].CenterProximity;
      ChessPiece cp = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(), c.center, c.r)) {
        if (
            (get_piece_type(dragged_piece.piece) == chess_board.turn) &&
            (get_piece_type(cp) != get_piece_type(dragged_piece.piece))
            ) {
          chess_board.squares[y][x].piece = dragged_piece.piece;
          piece_placed = true;
          change_chess_board_turn();
          break;
        }
      }
    }
    if (piece_placed) break;
  }
  if (!piece_placed)
    orig_dragged_piece->piece = dragged_piece.piece;
  piece_placed = false; // Reset
}

void draw_drag_and_place(void)
{
  if (!dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        dragged_piece = chess_board.squares[y][x];
        orig_dragged_piece = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), dragged_piece.rect) && (dragged_piece.piece != NO_PIECE)) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragging_piece = true;
            chess_board.squares[y][x].piece = NO_PIECE;
            break;
          }
        }
      }
      if (dragging_piece) break;
    }
  }

  if (dragging_piece) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle rect = {
      .x      = mouse_pos.x - dragged_piece.rect.width / 2,
      .y      = mouse_pos.y - dragged_piece.rect.height / 2,
      .width  = dragged_piece.rect.width,
      .height = dragged_piece.rect.height
    };
    ChessPiece type = dragged_piece.piece;
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
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) && (square->piece != NO_PIECE)) {
        hovering_piece = true;
        break;
      }
    }
    if (hovering_piece) break;
  }

  draw_drag_and_place();

  SetMouseCursor(
      hovering_piece || dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
  hovering_piece = false; // Reset
}
