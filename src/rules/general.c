#include "rules/general.h"
#include "rules/pieces.h"
#include "core.h"
#include "init.h"
#include <stdlib.h>

bool verify_check(ChessPieceColor verify_color)
{
  bool check = false;
  ChessSquare *ally_king = NULL;

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *s = &chess_board.squares[y][x];
      if (s->piece.color == verify_color && s->piece.type == KING)
        ally_king = s;
      if (ally_king != NULL) break;
    }
    if (ally_king != NULL) break;
  }

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *src = &chess_board.squares[y][x];
      if (src->piece.color != chess_board.color_turn) {
        check = is_legal_move(src, ally_king, src->piece.type);
      }
      if (check) break;
    }
    if (check) break;
  }
  return check;
}

void verify_if_any_legal_move(ChessPieceColor verify_color)
{
  bool no_legal_move = true;

  // Iterate over all enemy pieces:
  // - try to make all the moves with them
  // - if they are legal and is still check, continue
  // - when no more enemy pieces are left to verify -> checkmate
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *enemy = &chess_board.squares[y][x];
      ChessPiece enemy_copy = chess_board.squares[y][x].piece;
      if (enemy->piece.color == verify_color) {
        for (int j = 0; j < NS; j++) {
          for (int i = 0; i < NS; i++) {
            ChessSquare *square = &chess_board.squares[j][i];
            ChessPiece square_copy = chess_board.squares[j][i].piece;

            if ((enemy->piece.color != square->piece.color) &&
                !capture_king(square) &&
                !still_on_src_square(enemy, square) &&
                is_legal_move(enemy, square, enemy->piece.type)) {
              // Add piece to square and empty enemy
              square->piece = enemy_copy;
              reset_chess_square(enemy);

              if (verify_check(square->piece.color)) {
                square->piece = square_copy;
                enemy->piece = enemy_copy;
              } else {
                square->piece = square_copy;
                enemy->piece = enemy_copy;
                no_legal_move = false;
                break;
              }
            }
          }
          if (!no_legal_move) break;
        }
      }
      if (!no_legal_move) break;
    }
    if (!no_legal_move) break;
  }
  if (no_legal_move) {
    if (chess_board.state.is_check) chess_board.result = CHECKMATE;
    else                            chess_board.result = DRAW;
  }
}

void gave_check(void)
{
  ChessSquare *enemy_king = NULL;

  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *s = &chess_board.squares[y][x];
      if (s->piece.color != chess_board.color_turn && s->piece.type == KING)
        enemy_king = s;
      if (enemy_king != NULL) break;
    }
    if (enemy_king != NULL) break;
  }

  if (is_legal_move(chess_board.moving.c_dest, enemy_king, chess_board.moving.c_dest->piece.type))
    chess_board.state.is_check = true;
}

bool valid_move(ChessSquare *src, ChessSquare *dest)
{
  return (correct_color_turn() &&
      !capture_ally(dest) &&
      !capture_king(dest) &&
      !still_on_src_square(src, dest) &&
      is_legal_move(src, dest, chess_board.moving.src_piece.type));
}

bool is_legal_move(ChessSquare *src, ChessSquare *dest, ChessPieceType type)
{
  bool legal_move = false;

  // Check if legal move
  switch (type) {
    case PAWN:
      legal_move = pawn_is_legal_move(src, dest);
      break;
    case BISHOP:
      legal_move = bishop_is_legal_move(src, dest);
      break;
    case KING:
      legal_move = king_is_legal_move(src, dest);
      break;
    case KNIGHT:
      legal_move = knight_is_legal_move(src, dest);
      break;
    case QUEEN:
      legal_move = queen_is_legal_move(src, dest);
      break;
    case ROOK:
      legal_move = rook_is_legal_move(src, dest);
      break;
    case NO_PIECE:
      break;
    case PIECE_COUNT:
      break;
    }
  return legal_move;
}

bool still_on_src_square(const ChessSquare *src, const ChessSquare *dest)
{
  return (src == dest);
}

bool capture_king(const ChessSquare *dest)
{
  return (dest->piece.type == KING);
}

bool capture_ally(const ChessSquare *dest)
{
  return (chess_board.moving.src_piece.color == dest->piece.color);
}

bool correct_color_turn(void)
{
  return (chess_board.moving.src_piece.color == chess_board.color_turn);
}
