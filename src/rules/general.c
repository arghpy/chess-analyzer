#include "rules/general.h"
#include "board.h"

bool still_on_src_square(float dx, float dy)
{
  return (dx == 0 && dy == 0);
}

bool capture_ally(void)
{
  return (chess_board.src_piece.color == chess_board.dest->piece.color);
}

bool correct_color_turn(void)
{
  return (chess_board.src_piece.color == chess_board.color_turn);
}
