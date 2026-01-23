#include "rules/bishop.h"
#include "board.h"
#include <math.h>
#include <stddef.h>

bool bishop_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int dx_sign = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
                -1 : 1;
  int dy_sign = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  // TODO: this is a general move rule - separate function
  if (dx == 0 && dy == 0) return false;

  if (dx == dy) {
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;

      ys += dy_sign;
      xs += dx_sign;
    }

    return true;
  } else return false;
}
