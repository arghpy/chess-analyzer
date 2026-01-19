#include "mouse.h"
#include "board.h"
#include "raylib.h"


bool mouse_hovering_piece = false;

void draw_mouse_cursor(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square = &chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square->rect) && (square->piece != NO_PIECE)) {
        mouse_hovering_piece = true;
        break;
      }
    }
    if (mouse_hovering_piece) break;
  }
  SetMouseCursor(
      mouse_hovering_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
  mouse_hovering_piece = false;
}
