#include "board.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

Texture2D chess_pieces[END_B] = {0};

ChessBoard chess_board = {0};
float SQUARE_SIZE = 0.0f;

typedef enum {
  LIGHT,
  DARK
} BoardColor;

static const Color square_color[] = {
  [LIGHT] = (Color) { 0xED, 0xD4, 0xAE, 0xFF },
  [DARK]  = (Color) { 0xB9, 0x89, 0x65, 0xFF }
};

ChessPieceType get_piece_color(ChessPiece piece)
{
  if (piece < END_W) return W;
  else if (piece > END_W && piece < END_B) return B;
  else return N;
}

void load_chess_pieces(void)
{
  chess_pieces[PAWN_B]   = LoadTexture("./assets/pieces/pb.png");
  chess_pieces[PAWN_W]   = LoadTexture("./assets/pieces/pw.png");
  chess_pieces[BISHOP_B] = LoadTexture("./assets/pieces/bb.png");
  chess_pieces[BISHOP_W] = LoadTexture("./assets/pieces/bw.png");
  chess_pieces[KING_B]   = LoadTexture("./assets/pieces/kb.png");
  chess_pieces[KING_W]   = LoadTexture("./assets/pieces/kw.png");
  chess_pieces[KNIGHT_B] = LoadTexture("./assets/pieces/nb.png");
  chess_pieces[KNIGHT_W] = LoadTexture("./assets/pieces/nw.png");
  chess_pieces[QUEEN_B]  = LoadTexture("./assets/pieces/qb.png");
  chess_pieces[QUEEN_W]  = LoadTexture("./assets/pieces/qw.png");
  chess_pieces[ROOK_B]   = LoadTexture("./assets/pieces/rb.png");
  chess_pieces[ROOK_W]   = LoadTexture("./assets/pieces/rw.png");

  for (int i = 0; i < END_B; i++)
    SetTextureFilter(chess_pieces[i], TEXTURE_FILTER_BILINEAR);
}

void unload_chess_pieces(void)
{
  for (int i = 0; i < END_B; i++)
    UnloadTexture(chess_pieces[i]);
}

void init_chess_board(void)
{
  chess_board.color_turn = W;
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.squares[y][x].color = square_color[(x + y) % 2];
      chess_board.squares[y][x].piece = NO_PIECE;
    }
  }

  // Black
  chess_board.squares[0][0].piece = ROOK_B;
  chess_board.squares[0][1].piece = KNIGHT_B;
  chess_board.squares[0][2].piece = BISHOP_B;
  chess_board.squares[0][3].piece = QUEEN_B;
  chess_board.squares[0][4].piece = KING_B;
  chess_board.squares[0][5].piece = BISHOP_B;
  chess_board.squares[0][6].piece = KNIGHT_B;
  chess_board.squares[0][7].piece = ROOK_B;

  // White
  chess_board.squares[7][0].piece = ROOK_W;
  chess_board.squares[7][1].piece = KNIGHT_W;
  chess_board.squares[7][2].piece = BISHOP_W;
  chess_board.squares[7][3].piece = QUEEN_W;
  chess_board.squares[7][4].piece = KING_W;
  chess_board.squares[7][5].piece = BISHOP_W;
  chess_board.squares[7][6].piece = KNIGHT_W;
  chess_board.squares[7][7].piece = ROOK_W;

  // Pawns
  for (int i = 0; i < NS; i++) {
    chess_board.squares[1][i].piece = PAWN_B;
    chess_board.squares[6][i].piece = PAWN_W;
  }
}

void scale_chess_board(void)
{
  SQUARE_SIZE = roundf(
      GetScreenWidth() < GetScreenHeight()
      ? GetScreenWidth() / NS
      : GetScreenHeight() / NS
  );

  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++) {
      chess_board.squares[y][x].rect = (Rectangle) {
        .x      = x * SQUARE_SIZE,
        .y      = y * SQUARE_SIZE,
        .width  = SQUARE_SIZE,
        .height = SQUARE_SIZE,
      };
      chess_board.squares[y][x].CenterProximity = (Circle) {
        .center = (Vector2) {
          .x = x * SQUARE_SIZE + SQUARE_SIZE / 2,
          .y = y * SQUARE_SIZE + SQUARE_SIZE / 2
        },
        .r = SQUARE_SIZE / 3,
      };
    }
}

void draw_board(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      DrawRectangleRec(chess_board.squares[y][x].rect, chess_board.squares[y][x].color);
}

void draw_board_coordinates(Font *font)
{
  char text[10];
  int square_spacing = 5;
  float font_size = roundf(SQUARE_SIZE * 0.3f);

  // Numbers
  for (int y = 0; y < NS; y++) {
    snprintf(text, sizeof(text), "%d", NS - y);

    Vector2 pos = {
      .x = chess_board.squares[y][0].rect.x + square_spacing,
      .y = chess_board.squares[y][0].rect.y + square_spacing
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[(y + 1) % 2]);
  }

  // Letters
  for (int x = 0; x < NS; x++) {
    snprintf(text, sizeof(text), "%c", 'a' + x);
    Vector2 text_size = MeasureTextEx(*font, text, font_size, 0);

    Vector2 pos = {
      .x = chess_board.squares[NS - 1][x].rect.x + (SQUARE_SIZE - text_size.x - square_spacing),
      .y = chess_board.squares[NS - 1][x].rect.y + (SQUARE_SIZE - text_size.y - square_spacing)
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[x % 2]);
  }
}

void draw_piece(Texture2D *piece, Rectangle *rect)
{
  Rectangle source = {
    0, 0,
    piece->width,
    piece->height
  };

  Rectangle dest = {
    rect->x,
    rect->y,
    SQUARE_SIZE,
    SQUARE_SIZE
  };

  DrawTexturePro(*piece, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void draw_chess_pieces(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++) {
      Rectangle *rect = &chess_board.squares[y][x].rect;
      ChessPiece type = chess_board.squares[y][x].piece;
      Texture2D *piece = &chess_pieces[type];
      draw_piece(piece, rect);
    }
}

void draw_chess_board(Font *font)
{
  scale_chess_board();
  draw_board();
  draw_board_coordinates(font);
  draw_chess_pieces();
}
