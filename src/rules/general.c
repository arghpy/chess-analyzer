#include "rules/general.h"

bool no_move(float dx, float dy)
{
  return (dx == 0 && dy == 0);
}

bool capture_ally(ChessSquare *src, ChessSquare *dest)
{
  return (src->piece.color == dest->piece.color);
}

bool correct_color_turn(ChessSquare *src)
{
  return (src->piece.color == chess_board.color_turn);
}
