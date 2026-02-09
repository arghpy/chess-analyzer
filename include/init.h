#ifndef INIT_H
#define INIT_H

#include "raylib.h"

#define NS 8

typedef enum {
  LIGHT_TILE,
  DARK_TILE
} BoardColor;

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
  N,
  W,
  B,
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
  bool hovering_piece;
  bool dragging_piece;
  bool placed_piece;
  bool promote;
  bool hovering_promotion;
  bool is_check;
  bool w_moved;
  bool b_moved;
  bool captured;
} States;

typedef enum {
  NONE,
  CHECKMATE,
  DRAW,
} Result;

typedef struct {
  bool allowed;
  ChessPieceColor allowed_by_color;
  ChessSquare *square;
} EnPassant;

typedef struct {
  ChessSquare *c_src;
  ChessSquare *p_src;
  ChessPiece src_piece;
  ChessPiece dest_piece;
  ChessSquare *c_dest;
  ChessSquare *p_dest;
  ChessPiece captured_piece;
} MovingPieces;

typedef struct {
  bool w_s_can_castle;
  bool w_l_can_castle;
  bool b_s_can_castle;
  bool b_l_can_castle;
} Castle;

typedef struct {
  ChessSquare squares[NS][NS];
  ChessPieceColor color_turn;
  States state;
  Castle castle;
  bool board_flipped;
  int halfmoves;
  int fullmoves;
  MovingPieces moving;
  EnPassant enpassant;
  Result result;
  ChessPieceColor won;
  ChessPieceColor lost;
} ChessBoard;

extern ChessBoard chess_board;
extern Texture2D chess_pieces_texture[];
extern const Color square_color[];
extern ChessSquare piece_promotions[];

bool load_starting_position(void);
void load_pawn_promotions(void);
bool load_chess_pieces(void);
void unload_chess_pieces(void);

#endif
