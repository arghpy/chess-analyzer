#include "init.h"
#include "protocols/fen.h"
#include <stdio.h>

ChessBoard chess_board = {0};

const Color square_color[] = {
  [LIGHT_TILE] = (Color) { 0xED, 0xD4, 0xAE, 0xFF },
  [DARK_TILE]  = (Color) { 0xB9, 0x89, 0x65, 0xFF }
};

Texture2D chess_pieces_texture[TEXTURE_COUNT] = {0};

ChessSquare piece_promotions[4] = {0};

bool load_starting_position(void)
{
  for (int y = 0; y < NS; y++)
    for (int x = 0; x < NS; x++)
      chess_board.squares[y][x].board_color = square_color[(x + y) % 2];

  return load_fen_position(FEN_START_POS);
}

void load_pawn_promotions(void)
{
  piece_promotions[0].piece.type = QUEEN;
  piece_promotions[1].piece.type = KNIGHT;
  piece_promotions[2].piece.type = ROOK;
  piece_promotions[3].piece.type = BISHOP;
}

bool load_chess_pieces(void)
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

  for (int i = 0; i < TEXTURE_COUNT; i++) {
    if (!IsTextureValid(chess_pieces_texture[i])) {
      fprintf(stderr, "Texture not valid: %s:%d.\n", __FILE__, __LINE__ - 3 - TEXTURE_COUNT + i);
      return false;
    }
    SetTextureFilter(chess_pieces_texture[i], TEXTURE_FILTER_BILINEAR);
  }
  return true;
}

void unload_chess_pieces(void)
{
  for (int i = 0; i < TEXTURE_COUNT; i++)
    UnloadTexture(chess_pieces_texture[i]);
}
