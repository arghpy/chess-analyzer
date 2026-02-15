#include "protocols/common.h"
#include <stdio.h>

char* get_piece_notation(ChessPiece piece)
{
  switch(piece.type) {
    case NO_PIECE: return "0";
    case PAWN:
      if (piece.color == W) return "P";
      else return "p";
    case BISHOP:
      if (piece.color == W) return "B";
      else return "b";
    case KING:
      if (piece.color == W) return "K";
      else return "k";
    case KNIGHT:
      if (piece.color == W) return "N";
      else return "n";
    case QUEEN:
      if (piece.color == W) return "Q";
      else return "q";
    case ROOK:
      if (piece.color == W) return "R";
      else return "r";
    case PIECE_COUNT: return NULL;
  }
  return NULL;
}
