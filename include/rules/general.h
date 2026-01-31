#ifndef RULE_GENERAL_H
#define RULE_GENERAL_H

#include <stdbool.h>
#include "init.h"

bool still_on_src_square(const ChessSquare *src, const ChessSquare *dest);
bool capture_king(const ChessSquare *dest);
bool capture_ally(const ChessSquare *dest);
bool correct_color_turn(void);

#endif
