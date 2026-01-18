#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"

#define NS 8

void draw_chess_board(Font *font);
void load_chess_pieces(void);
void unload_chess_pieces(void);
void init_chess_board(void);
void draw_piece(Texture2D *piece, Rectangle *rect);

typedef enum {
  PAWN_B,
  PAWN_W,
  BISHOP_B,
  BISHOP_W,
  KING_B,
  KING_W,
  KNIGHT_B,
  KNIGHT_W,
  QUEEN_B,
  QUEEN_W,
  ROOK_B,
  ROOK_W,
  NO_PIECE,

  PIECE_COUNT
} ChessPiece;

typedef struct {
  Rectangle rect;
  ChessPiece piece;
  Color color;
} ChessSquare;

typedef struct {
  ChessSquare squares[NS][NS];
} ChessBoard;

extern ChessBoard chess_board;
extern Texture2D chess_pieces[PIECE_COUNT];
extern float SQUARE_SIZE;

#endif
