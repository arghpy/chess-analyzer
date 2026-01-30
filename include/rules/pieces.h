#ifndef RULE_PIECES_H
#define RULE_PIECES_H

#include "init.h"
#include <stdbool.h>

bool knight_is_legal_move(void);
bool bishop_is_legal_move(void);
bool rook_is_legal_move(void);
bool queen_is_legal_move(void);
bool king_is_legal_move(void);
bool pawn_is_legal_move(void);
void promote_pawn(ChessSquare *promotion_square);

#endif
