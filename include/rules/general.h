#ifndef RULE_GENERAL_H
#define RULE_GENERAL_H

#include <stdbool.h>
#include "init.h"

bool is_legal_move(ChessSquare *src, ChessSquare *dest, const ChessPiece piece);
bool valid_move(ChessSquare *src, ChessSquare *dest, const ChessPiece src_piece);
bool in_check(ChessPieceColor verify_color);
void verify_if_any_legal_move(ChessPieceColor verify_color);
bool still_on_src_square(const ChessSquare *src, const ChessSquare *dest);
bool capture_king(const ChessSquare *dest);
bool capture_ally(const ChessSquare *dest, const ChessPiece src_piece);
bool correct_color_turn(const ChessPiece src_piece);

#endif
