#ifndef RULE_GENERAL_H
#define RULE_GENERAL_H

#include <stdbool.h>
#include "init.h"

bool is_legal_move(ChessSquare *src, ChessSquare *dest, ChessPieceType type);
bool valid_move(ChessSquare *src, ChessSquare *dest);
bool verify_check(ChessPieceColor verify_color);
void gave_check(void);
bool verify_checkmate(ChessPieceColor verify_color);
bool still_on_src_square(const ChessSquare *src, const ChessSquare *dest);
bool capture_king(const ChessSquare *dest);
bool capture_ally(const ChessSquare *dest);
bool correct_color_turn(void);

#endif
