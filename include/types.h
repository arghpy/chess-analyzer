#ifndef TYPES_H
#define TYPES_H

#include "defines.h"
#include "raylib.h"
#include "utils.h"

typedef enum {
  PLAYING,
  PROMOTING,
  REWINDING,
  DRAW,
  CHECKMATE,
} GameState;

typedef enum {
  MAIN,
  ANALYSIS,
} MenuState;

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
  ChessSquare* src;
  ChessSquare* dest;
} SrcDestChessSquares;

typedef struct {
  bool hovering_piece;
  bool piece_placed;
  bool promote;
  bool promotion_done;
  bool captured;
  bool verify;
} States;

typedef struct {
  bool done;
  bool allowed;
  ChessPieceColor allowed_by_color;
  ChessSquare *square;
} EnPassant;

typedef enum {
  NO,
  SHORT,
  LONG,
} Castled;

typedef struct {
  bool w_s_can_castle;
  bool w_l_can_castle;
  bool b_s_can_castle;
  bool b_l_can_castle;
  Castled castled;
} Castle;

typedef enum {
  NOTHING,
  CAPTURE,
  CASTLE,
  GAME_END,
  GAME_START,
  ILLEGAL,
  MOVE,
  MOVE_CHECK,
  PROMOTE,
  SOUNDS_COUNT
} GameSound;

typedef struct {
  ChessSquare squares[NS][NS];
  ChessPieceColor color_turn;
  States state;
  Castle castle;
  bool board_flipped;
  int halfmoves;
  int fullmoves;
  ChessPiece captured_piece;
  EnPassant enpassant;
  GameSound action_sound;
} ChessBoard;

typedef struct {
  ChessSquare *src;
  ChessSquare *dest;
  char san[8];
  char uci_move[6];
  ChessPiece piece;
  GameSound sound;
  char fen[FEN_MAX_LEN];
  int move_nr;
} ChessMove;

typedef struct {
  Vector2 direction;
  Vector2 src;
  Vector2 v1;
  Vector2 v2;
  Vector2 v3;
  Color color;
} Arrow;

typedef ut_da_declare(Arrow) Arrows;

typedef struct {
  char fen_p[FEN_MAX_LEN];
  int c;
} Position;

typedef ut_da_declare(Position) Positions;

ut_ll_declare(ChessMoveNode, ChessMove);

#endif
