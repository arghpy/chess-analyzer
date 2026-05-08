#ifndef INIT_H
#define INIT_H

#include "raylib.h"
#include "sound.h"
#include "utils.h"


#define WINDOW_FACTOR 67.5
#define WINDOW_WIDTH  (WINDOW_FACTOR * 16)
#define WINDOW_HEIGHT (WINDOW_FACTOR * 9)

#define NS 8
#define SQUARE_SIZE (((WINDOW_WIDTH) < (WINDOW_HEIGHT)) ? ((WINDOW_WIDTH) / (NS)) : ((WINDOW_HEIGHT) / (NS)))

typedef enum {
  LIGHT_TILE,
  DARK_TILE
} BoardColor;

typedef enum {
  LIGHT_OCCUPIED_TILE,
  DARK_OCCUPIED_TILE
} BoardColorOccupied;

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
  ChessPiece piece;
  GameSound sound;
  char fen[86];
  int move_nr;
} ChessMove;

ut_ll_declare(ChessMoveNode, ChessMove);

extern ChessMoveNode *ll_chess_move_head;
extern ChessMove moving_piece;
extern ChessMoveNode *ll_chess_move_tail;
extern ChessMoveNode *ll_chess_move_current;
extern ChessBoard chess_board;
extern Texture2D chess_pieces_texture[];
extern const Color square_color[];
extern const Color occupied_square_color[];
extern ChessSquare piece_promotions[4];

bool initialize_chess_board(void);
void load_pawn_promotions(void);
bool load_chess_pieces(void);
void unload_chess_pieces(void);

#endif
