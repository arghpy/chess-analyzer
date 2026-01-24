#include "rules/pieces.h"
#include "rules/general.h"
#include "board.h"
#include <math.h>
#include <stddef.h>

bool knight_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  if (no_move(dx, dy)) return false;
  if ((dx == SQUARE_SIZE && dy == 2*SQUARE_SIZE) || (dx == 2*SQUARE_SIZE && dy == SQUARE_SIZE))
    return true;
  else return false;
}

bool bishop_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int x_step = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (no_move(dx, dy)) return false;

  if (dx == dy) {
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;

      ys += y_step;
      xs += x_step;
    }

    return true;
  } else return false;
}
