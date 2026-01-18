#include "board.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define NS 8

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

  PIECE_COUNT
} ChessPiece;

Texture2D chess_pieces[PIECE_COUNT] = {0};

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

void unload_chess_pieces(void)
{
  for (int i = 0; i < PIECE_COUNT; i++)
    UnloadTexture(chess_pieces[i]);
}

void load_chess_pieces(void)
{
  chess_pieces[PAWN_B]   = LoadTexture("./thirdparty/assets/pieces/pb.png");
  chess_pieces[PAWN_W]   = LoadTexture("./thirdparty/assets/pieces/pw.png");
  chess_pieces[BISHOP_B] = LoadTexture("./thirdparty/assets/pieces/bb.png");
  chess_pieces[BISHOP_W] = LoadTexture("./thirdparty/assets/pieces/bw.png");
  chess_pieces[KING_B]   = LoadTexture("./thirdparty/assets/pieces/kb.png");
  chess_pieces[KING_W]   = LoadTexture("./thirdparty/assets/pieces/kw.png");
  chess_pieces[KNIGHT_B] = LoadTexture("./thirdparty/assets/pieces/nb.png");
  chess_pieces[KNIGHT_W] = LoadTexture("./thirdparty/assets/pieces/nw.png");
  chess_pieces[QUEEN_B]  = LoadTexture("./thirdparty/assets/pieces/qb.png");
  chess_pieces[QUEEN_W]  = LoadTexture("./thirdparty/assets/pieces/qw.png");
  chess_pieces[ROOK_B]   = LoadTexture("./thirdparty/assets/pieces/rb.png");
  chess_pieces[ROOK_W]   = LoadTexture("./thirdparty/assets/pieces/rw.png");

  for (int i = 0; i < PIECE_COUNT; i++)
    SetTextureFilter(chess_pieces[i], TEXTURE_FILTER_BILINEAR);
}

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

void draw_piece(Texture2D piece, Rectangle rect)
{
  float square_size = rect.width;

  Rectangle source = {
    0, 0,
    piece.width,
    piece.height
  };

  Rectangle dest = {
    rect.x,
    rect.y,
    square_size,
    square_size
  };

  DrawTexturePro(piece, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void draw_initial_position(void)
{
  // Black
  draw_piece(chess_pieces[ROOK_B],   chess_board.chess_squares[0][0].r);
  draw_piece(chess_pieces[KNIGHT_B], chess_board.chess_squares[0][1].r);
  draw_piece(chess_pieces[BISHOP_B], chess_board.chess_squares[0][2].r);
  draw_piece(chess_pieces[QUEEN_B],  chess_board.chess_squares[0][3].r);
  draw_piece(chess_pieces[KING_B],   chess_board.chess_squares[0][4].r);
  draw_piece(chess_pieces[BISHOP_B], chess_board.chess_squares[0][5].r);
  draw_piece(chess_pieces[KNIGHT_B], chess_board.chess_squares[0][6].r);
  draw_piece(chess_pieces[ROOK_B],   chess_board.chess_squares[0][7].r);

  // White
  draw_piece(chess_pieces[ROOK_W],   chess_board.chess_squares[7][0].r);
  draw_piece(chess_pieces[KNIGHT_W], chess_board.chess_squares[7][1].r);
  draw_piece(chess_pieces[BISHOP_W], chess_board.chess_squares[7][2].r);
  draw_piece(chess_pieces[QUEEN_W],  chess_board.chess_squares[7][3].r);
  draw_piece(chess_pieces[KING_W],   chess_board.chess_squares[7][4].r);
  draw_piece(chess_pieces[BISHOP_W], chess_board.chess_squares[7][5].r);
  draw_piece(chess_pieces[KNIGHT_W], chess_board.chess_squares[7][6].r);
  draw_piece(chess_pieces[ROOK_W],   chess_board.chess_squares[7][7].r);

  // Draw pawns
  for (int i = 0; i < NS; i++) {
    draw_piece(chess_pieces[PAWN_B], chess_board.chess_squares[1][i].r);
    draw_piece(chess_pieces[PAWN_W], chess_board.chess_squares[6][i].r);
  }
}

void draw_chess_board(Font *font)
{
  update_chess_board();
  draw_board();
  draw_coordinates(font);
  draw_initial_position();
}
