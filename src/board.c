#include "board.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

Texture2D chess_pieces_texture[TEXTURE_COUNT] = {0};

ChessBoard chess_board = {0};
float SQUARE_SIZE = 0.0f;

typedef enum {
  LIGHT_TILE,
  DARK_TILE
} BoardColor;

static const Color square_color[] = {
  [LIGHT_TILE] = (Color) { 0xED, 0xD4, 0xAE, 0xFF },
  [DARK_TILE]  = (Color) { 0xB9, 0x89, 0x65, 0xFF }
};

void flip_board(void)
{
  for (int y = 0; y < NS / 2; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare tmp = chess_board.squares[y][x];
      chess_board.squares[y][x] = chess_board.squares[NS - 1 - y][NS - 1 - x];
      chess_board.squares[NS - 1 - y][NS - 1 - x] = tmp;
    }
  }
}

void load_chess_pieces(void)
{
  chess_pieces_texture[PAWN_W]   = LoadTexture("./assets/pieces/pw.png");
  chess_pieces_texture[PAWN_B]   = LoadTexture("./assets/pieces/pb.png");
  chess_pieces_texture[BISHOP_W] = LoadTexture("./assets/pieces/bw.png");
  chess_pieces_texture[BISHOP_B] = LoadTexture("./assets/pieces/bb.png");
  chess_pieces_texture[KING_W]   = LoadTexture("./assets/pieces/kw.png");
  chess_pieces_texture[KING_B]   = LoadTexture("./assets/pieces/kb.png");
  chess_pieces_texture[KNIGHT_W] = LoadTexture("./assets/pieces/nw.png");
  chess_pieces_texture[KNIGHT_B] = LoadTexture("./assets/pieces/nb.png");
  chess_pieces_texture[QUEEN_W]  = LoadTexture("./assets/pieces/qw.png");
  chess_pieces_texture[QUEEN_B]  = LoadTexture("./assets/pieces/qb.png");
  chess_pieces_texture[ROOK_W]   = LoadTexture("./assets/pieces/rw.png");
  chess_pieces_texture[ROOK_B]   = LoadTexture("./assets/pieces/rb.png");

  for (int i = 0; i < TEXTURE_COUNT; i++)
    SetTextureFilter(chess_pieces_texture[i], TEXTURE_FILTER_BILINEAR);
}

void unload_chess_pieces(void)
{
  for (int i = 0; i < TEXTURE_COUNT; i++)
    UnloadTexture(chess_pieces_texture[i]);
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}

// TODO: load starting position based on FEN
void load_starting_position(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      chess_board.squares[y][x].board_color = square_color[(x + y) % 2];
      reset_chess_square(&chess_board.squares[y][x]);
    }
  }

  // Black
  chess_board.squares[0][0].piece.type = ROOK;
  chess_board.squares[0][1].piece.type = KNIGHT;
  chess_board.squares[0][2].piece.type = BISHOP;
  chess_board.squares[0][3].piece.type = QUEEN;
  chess_board.squares[0][4].piece.type = KING;
  chess_board.squares[0][5].piece.type = BISHOP;
  chess_board.squares[0][6].piece.type = KNIGHT;
  chess_board.squares[0][7].piece.type = ROOK;

  for (int x = 0; x < NS; x++)
    chess_board.squares[0][x].piece.color = B;

  // White
  chess_board.squares[7][0].piece.type = ROOK;
  chess_board.squares[7][1].piece.type = KNIGHT;
  chess_board.squares[7][2].piece.type = BISHOP;
  chess_board.squares[7][3].piece.type = QUEEN;
  chess_board.squares[7][4].piece.type = KING;
  chess_board.squares[7][5].piece.type = BISHOP;
  chess_board.squares[7][6].piece.type = KNIGHT;
  chess_board.squares[7][7].piece.type = ROOK;

  for (int x = 0; x < NS; x++)
    chess_board.squares[7][x].piece.color = W;

  // Pawns
  for (int i = 0; i < NS; i++) {
    chess_board.squares[1][i].piece.type = PAWN;
    chess_board.squares[1][i].piece.color = B;

    chess_board.squares[6][i].piece.type = PAWN;
    chess_board.squares[6][i].piece.color = W;
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
      chess_board.squares[y][x].center_proximity = (Circle) {
        .center = (Vector2) {
          .x = x * SQUARE_SIZE + SQUARE_SIZE / 2,
          .y = y * SQUARE_SIZE + SQUARE_SIZE / 2
        },
        .r = SQUARE_SIZE / 2.5f,
      };
    }
}

void draw_board(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      DrawRectangleRec(chess_board.squares[y][x].rect, chess_board.squares[y][x].board_color);
}

void draw_board_coordinates(Font *font)
{
  char text[10];
  int square_spacing = 5;
  float font_size = roundf(SQUARE_SIZE * 0.3f);

  // Numbers
  for (int y = 0; y < NS; y++) {
    snprintf(text, sizeof(text), "%d", chess_board.flipped ? y + 1 : NS - y);

    Vector2 pos = {
      .x = chess_board.squares[y][0].rect.x + square_spacing,
      .y = chess_board.squares[y][0].rect.y + square_spacing
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[(y + 1) % 2]);
  }

  // Letters
  for (int x = 0; x < NS; x++) {
    snprintf(text, sizeof(text), "%c", chess_board.flipped ? 'h' - x : 'a' + x);

    Vector2 text_size = MeasureTextEx(*font, text, font_size, 0);

    Vector2 pos = {
      .x = chess_board.squares[NS - 1][x].rect.x + (SQUARE_SIZE - text_size.x - square_spacing),
      .y = chess_board.squares[NS - 1][x].rect.y + (SQUARE_SIZE - text_size.y - square_spacing)
    };
    DrawTextEx(*font, text, pos, font_size, 0, square_color[x % 2]);
  }
}

void draw_piece(const ChessSquare *square)
{
  float piece_size = SQUARE_SIZE * 0.9;

  Rectangle rect = square->rect;
  Texture2D *piece_texture = &chess_pieces_texture[2*(square->piece.type - 1) + square->piece.color];

  Rectangle source = {
    0, 0,
    piece_texture->width,
    piece_texture->height
  };

  Rectangle dest = {
    rect.x + (SQUARE_SIZE / 2 - piece_size / 2),
    rect.y + (SQUARE_SIZE / 2 - piece_size / 2),
    piece_size,
    piece_size
  };

  DrawTexturePro(*piece_texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

void draw_chess_pieces(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessPieceType type   = chess_board.squares[y][x].piece.type;
      if (type != NO_PIECE) {
        // Account for NO_PIECE
        draw_piece(&chess_board.squares[y][x]);
      }
    }
  }
}

void draw_chess_board(Font *font)
{
  scale_chess_board();
  draw_board();
  draw_board_coordinates(font);
  draw_chess_pieces();
}
