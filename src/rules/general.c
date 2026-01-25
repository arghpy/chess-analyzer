#include "rules/general.h"

bool still_on_src_square(float dx, float dy)
{
  return (dx == 0 && dy == 0);
}

bool capture_ally(ChessPiece *src, ChessPiece *dest)
{
  return (src->color == dest->color);
}

bool correct_color_turn(ChessPiece *piece)
{
  return (piece->color == chess_board.color_turn);
}
