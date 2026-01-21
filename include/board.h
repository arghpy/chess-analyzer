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
  PAWN_W,
  BISHOP_W,
  KING_W,
  KNIGHT_W,
  QUEEN_W,
  ROOK_W,

  END_W,

  PAWN_B,
  BISHOP_B,
  KING_B,
  KNIGHT_B,
  QUEEN_B,
  ROOK_B,

  END_B,

  NO_PIECE,
} ChessPiece;

typedef enum {
  W,
  B,
  N
} ChessPieceType;

ChessPieceType get_piece_color(ChessPiece piece);

typedef struct {
  Vector2 center;
  float r;
} Circle;

typedef struct {
  Rectangle rect;
  ChessPiece piece;
  Color color;
  Circle CenterProximity;
} ChessSquare;

typedef struct {
  ChessSquare squares[NS][NS];
  ChessPieceType color_turn;
  ChessSquare *src;
  ChessSquare src_copy;
  ChessSquare *dst;
  bool hovering_piece;
  bool dragging_piece;
  bool piece_placed;
} ChessBoard;

extern ChessBoard chess_board;
extern Texture2D chess_pieces[END_B];
extern float SQUARE_SIZE;

#endif
