#include "input.h"
#include "core.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "init.h"
#include "render.h"
#include <stdio.h>

void process_keyboard_events(void)
{
  if (IsKeyPressed(KEY_SPACE)) {
    chess_board.board_flipped = !chess_board.board_flipped;
    flip_board();
  }

  // Color square red on right mouse click
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect)) {
          if (!ColorIsEqual(s->board_color, RED_SQUARE))
            s->board_color = RED_SQUARE;
          else {
            s->board_color = square_color[(x + y) % 2];
            if (s == chess_board.moving.prev_src || s == chess_board.moving.prev_dest)
              s->board_color  = color_occupied_square(s);
          }
        }
      }
    }
  }
}
