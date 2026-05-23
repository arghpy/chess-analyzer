#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include "init.h"

typedef enum {
  PLAYING,
  PROMOTING,
  REWINDING,
  DRAW,
  CHECKMATE,
} GameState;

typedef enum {
  MAIN,
  ANALYSIS,
} MenuState;

extern GameState game_state;
extern MenuState menu_state;
void process_menu_states(const Font* general_font, const Font* big_font);
void reset_chess_square(ChessSquare *square);
void place_piece(void);
void process_game_states(const Font* font);
void advance_game_parameters(void);
void reset_square_color(ChessSquare *s);
void reset_square_color_chess_move(ChessMoveNode *node);
void change_chess_board_turn(void);

#endif
