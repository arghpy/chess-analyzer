#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "init.h"
#include "types.h"

extern Arrows drawn_arrows;

void draw_paste_fen_window(const Font* font);
void draw_paste_pgn_window(const Font* font);
void draw_import_fen_pgn_button(const Font* font);
void check_pieces_hovering(void);
bool is_dragging_piece(void);
void draw_arrow(const Arrow* arrow);
void add_arrow(const ChessSquare* src, const ChessSquare* dest);
void draw_title(const Font* font);
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
