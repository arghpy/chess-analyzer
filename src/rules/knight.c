#include "rules/knight.h"
#include "board.h"
#include <math.h>

bool knight_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  if (dx == 0 && dy == 0) return false;
  if ((dx == SQUARE_SIZE && dy == 2*SQUARE_SIZE) || (dx == 2*SQUARE_SIZE && dy == SQUARE_SIZE))
    return true;
  else return false;
}
