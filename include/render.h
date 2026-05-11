#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "init.h"

#define background_color ((Color){0x18, 0x18, 0x18, 0xFF})
#define RED_SQUARE       CLITERAL(Color){226, 105, 85, 255}

// extern float SQUARE_SIZE;

void reset_colors_for_current_move(void);
void draw_copy_pgn_button(const Font* font);
void draw_copy_fen_button(const Font* font);
void draw_san_window(const Font *general_font);
void set_mouse_cursor(void);
void draw_moving_piece(void);
void draw_result(const Font *font, const char* result);
Color color_occupied_square(const ChessSquare *s);
void draw_piece(const ChessSquare *square);
void draw_chess_board(const Font *font);
void flip_board(void);

#endif
