#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include "init.h"

void increment_game_states(void);
void reset_chess_square(ChessSquare *square);
void place_piece(void);

#endif
