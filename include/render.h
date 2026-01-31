#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "init.h"

extern float SQUARE_SIZE;

Color color_occupied_square(const ChessSquare *s);
void draw_piece(const ChessSquare *square);
void draw_chess_board(Font *font);
void flip_board(void);

#endif
