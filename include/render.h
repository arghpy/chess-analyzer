#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "init.h"

#define background_color   (Color){0x18, 0x18, 0x18, 0xFF}
#define arrow_color        (Color){0xE5, 0x9E, 0x23, 0xD9}
#define paste_window_color (Color){0x37, 0x36, 0x33, 0xFF}
#define button_color       BROWN

// extern float SQUARE_SIZE;

typedef struct {
  Vector2 direction;
  Vector2 src;
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  Color color;
} Arrow;

typedef ut_da_declare(Arrow) Arrows;
extern Arrows drawn_arrows;

void draw_paste_fen_window(const Font* font);
void draw_paste_pgn_window(const Font* font);
void draw_load_fen_pgn_button(const Font* font);
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
