#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include "init.h"

typedef enum {
  PLAYING,
  PROMOTING,
  DRAW,
  CHECKMATE,
} GameState;

extern GameState game_state;
void increment_chess_states(void);
void reset_chess_square(ChessSquare *square);
void place_piece(void);
void process_game_states(const Font* font);
void advance_game_parameters(void);
void reset_square_color_chess_move(ChessMoveNode *node);
void change_chess_board_turn(void);

#endif
