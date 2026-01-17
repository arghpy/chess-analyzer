#include "board.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define NS 8

typedef struct {
  Rectangle r;
  Color c;
} ChessSquare;

typedef struct {
  ChessSquare chess_squares[NS][NS];
} ChessBoard;

ChessBoard chess_board = {0};

typedef enum {
  LIGHT,
  DARK
} BoardColor;

static const Color square_color[] = {
  [LIGHT] = (Color) { 0xED, 0xD4, 0xAE, 0xFF },
  [DARK]  = (Color) { 0xB9, 0x89, 0x65, 0xFF }
};

void update_chess_board(void)
{
  float square_size = roundf(
      GetScreenWidth() < GetScreenHeight()
      ? GetScreenWidth() / NS
      : GetScreenHeight() / NS
  );

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.chess_squares[y][x].r = (Rectangle) {
        .x      = x * square_size,
        .y      = y * square_size,
        .width  = square_size,
        .height = square_size,
      };
      chess_board.chess_squares[y][x].c = square_color[(x + y) % 2];
    }
  }
}

void draw_coordinates(Font *font)
{
  float square_size = chess_board.chess_squares[0][0].r.width;

  char text[10];
  int square_spacing = 5;
  float font_size = roundf(square_size * 0.3f);

  // Numbers
  for (int y = 0; y < NS; y++) {
    snprintf(text, sizeof(text), "%d", NS - y);

    Vector2 pos = {
      .x = chess_board.chess_squares[y][0].r.x + square_spacing,
      .y = chess_board.chess_squares[y][0].r.y + square_spacing
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[(y + 1) % 2]);
  }

  // Letters
  for (int x = 0; x < NS; x++) {
    snprintf(text, sizeof(text), "%c", 'a' + x);
    Vector2 text_size = MeasureTextEx(*font, text, font_size, 0);

    Vector2 pos = {
      .x = chess_board.chess_squares[NS - 1][x].r.x + (square_size - text_size.x - square_spacing),
      .y = chess_board.chess_squares[NS - 1][x].r.y + (square_size - text_size.y - square_spacing)
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[x % 2]);
  }
}

void draw_board(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++)
      DrawRectangleRec(chess_board.chess_squares[y][x].r, chess_board.chess_squares[y][x].c);
  }
}

void draw_chess_board(Font *font)
{
  create_chess_board();
  update_chess_board();
  draw_board();
  draw_coordinates(font);
}
