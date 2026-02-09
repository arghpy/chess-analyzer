#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "init.h"

#define background_color ((Color){0x18, 0x18, 0x18, 0xFF})

extern float SQUARE_SIZE;

void draw_result(const Font *font);
Color color_occupied_square(const ChessSquare *s);
void draw_piece(const ChessSquare *square);
void draw_chess_board(const Font *font);
void flip_board(void);

#endif
