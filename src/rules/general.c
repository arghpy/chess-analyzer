#include "rules/general.h"
#include "init.h"

bool still_on_src_square(const ChessSquare *src, const ChessSquare *dest)
{
  return (src == dest);
}

bool capture_king(const ChessSquare *dest)
{
  return (dest->piece.type == KING);
}

bool capture_ally(const ChessSquare *dest)
{
  return (chess_board.src_piece.color == dest->piece.color);
}

bool correct_color_turn(void)
{
  return (chess_board.src_piece.color == chess_board.color_turn);
}
