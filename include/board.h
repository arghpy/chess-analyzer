#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"

#define NS 8

typedef enum {
  PAWN_W,
  PAWN_B,
  BISHOP_W,
  BISHOP_B,
  KING_W,
  KING_B,
  KNIGHT_W,
  KNIGHT_B,
  QUEEN_W,
  QUEEN_B,
  ROOK_W,
  ROOK_B,

  TEXTURE_COUNT
} ChessPieceTexture;

typedef enum {
  NO_PIECE,
  PAWN,
  BISHOP,
  KING,
  KNIGHT,
  QUEEN,
  ROOK,

  PIECE_COUNT
} ChessPieceType;

typedef enum {
  W,
  B,
  N
} ChessPieceColor;

typedef struct {
  ChessPieceType type;
  ChessPieceColor color;
} ChessPiece;

typedef struct {
  Vector2 center;
  float r;
} Circle;

typedef struct {
  Rectangle rect;
  ChessPiece piece;
  Circle center_proximity;
  Color board_color;
} ChessSquare;

typedef struct {
  ChessSquare squares[NS][NS];
  ChessPieceColor color_turn;
  ChessSquare *src;
  ChessPiece src_piece;
  ChessSquare *dest;
  bool hovering_piece;
  bool dragging_piece;
  bool piece_placed;
  bool flipped;
  bool w_s_can_castle;
  bool w_l_can_castle;
  bool b_s_can_castle;
  bool b_l_can_castle;
  bool enpassant_allowed;
  ChessSquare *enpassant_allowed_by;
  bool promote;
  ChessSquare promotions[4];
  bool hovering_promotion;
} ChessBoard;

extern ChessBoard chess_board;
extern Texture2D chess_pieces_texture[TEXTURE_COUNT];
extern float SQUARE_SIZE;

void draw_piece(const ChessSquare *square);
void draw_chess_board(Font *font);
void load_pawn_promotions(void);
void load_chess_pieces(void);
void unload_chess_pieces(void);
void load_starting_position(void);
void flip_board(void);
void reset_chess_square(ChessSquare *square);

#endif
