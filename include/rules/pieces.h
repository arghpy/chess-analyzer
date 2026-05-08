#ifndef RULE_PIECES_H
#define RULE_PIECES_H

#include "init.h"
#include <stdbool.h>

ChessPiece select_for_promotion(void);
bool knight_is_legal_move(const ChessSquare *src, const ChessSquare *dest);
bool bishop_is_legal_move(const ChessSquare *src, const ChessSquare *dest);
bool rook_is_legal_move(const ChessSquare *src, const ChessSquare *dest, const ChessPiece src_piece);
bool queen_is_legal_move(const ChessSquare *src, const ChessSquare *dest);
bool king_is_legal_move(const ChessSquare *src, const ChessSquare *dest, const ChessPiece src_piece);
bool pawn_is_legal_move(const ChessSquare *src, ChessSquare *dest, const ChessPiece src_piece);
void draw_promotion_pieces(void);

#endif
