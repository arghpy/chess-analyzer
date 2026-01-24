#ifndef RULE_GENERAL_H
#define RULE_GENERAL_H

#include <stdbool.h>
#include "board.h"

bool no_move(float dx, float dy);
bool capture_ally(ChessPiece *src, ChessPiece *dest);
bool correct_color_turn(ChessPiece *piece);

#endif
