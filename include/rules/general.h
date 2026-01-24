#ifndef RULE_GENERAL_H
#define RULE_GENERAL_H

#include <stdbool.h>
#include "board.h"

bool no_move(float dx, float dy);
bool capture_ally(ChessSquare *src, ChessSquare *dest);
bool correct_color_turn(ChessSquare *src);

#endif
