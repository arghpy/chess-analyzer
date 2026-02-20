#include "protocols/san.h"
#include "core.h"
#include "protocols/common.h"
#include "init.h"
#include "utils.h"
#include "rules/general.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef ut_da_declare(ChessSquare*) PossibleMoves;

void generate_san(void)
{
  // Write SAN
  char *row    = "abcdefgh";
  char *column = "12345678";

  ptrdiff_t s_index = chess_board.moving.c_src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.moving.c_dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (!chess_board.board_flipped) {
    ys = NS - ys - 1;
    yd = NS - yd - 1;
  } else {
    xs = NS - xs - 1;
    xd = NS - xd - 1;
  }
  char pgn[20] = {0};
  char move[20] = {0};

  if (chess_board.state.w_moved && !chess_board.state.b_moved)
    snprintf(pgn, sizeof(pgn), "\n%d.", chess_board.fullmoves);

  strcat(move, " ");
  if (chess_board.castle.castled != NO) {
    if      (chess_board.castle.castled == SHORT) strcat(move, "O-O");
    else if (chess_board.castle.castled == LONG)  strcat(move, "O-O-O");
    if (in_check(chess_board.color_turn)) {
      if (chess_board.result == CHECKMATE) strcat(move, "#");
      else strcat(move, "+");
    }
  } else {
    if (chess_board.moving.src_piece.type != PAWN) {
      char* piece_notation = get_piece_notation(chess_board.moving.src_piece);
      char upper_piece_notation = toupper((unsigned char) piece_notation[0]);
      size_t len = strlen(move);
      move[len] = upper_piece_notation;
      move[len+1] = '\0';

      PossibleMoves ps = {0};
      char disambiguity[3] = {0};

      // Main iterator is on column in order to prioritize column notation
      for (int x = 0; x < NS; x++) {
        for (int y = 0; y < NS; y++) {
          ChessSquare *s = &chess_board.squares[y][x];
          ChessSquare s_copy = chess_board.squares[y][x];

          if (s == chess_board.moving.c_src) continue;
          if (s->piece.color == chess_board.moving.src_piece.color &&
              s->piece.type == chess_board.moving.src_piece.type &&
              is_legal_move(s, chess_board.moving.c_dest, s->piece)) {
            chess_board.moving.c_dest->piece = s->piece;
            reset_chess_square(s);
            if (!in_check(chess_board.moving.src_piece.color)) {
              s->piece = s_copy.piece;
              ut_da_push(&ps, s);
            } else s->piece = s_copy.piece;
          }
        }
      }

      // First time see if is possible to add the row
      for (size_t i = 0; i < ps.count; i++) {
        ChessSquare *possible_piece = ps.items[i];
        if (possible_piece != NULL) {
          ptrdiff_t p_index = possible_piece - &chess_board.squares[0][0];
          int yp = p_index / NS;
          int xp = p_index % NS;

          if (!chess_board.board_flipped) yp = NS - yp - 1;
          else xp = NS - xp - 1;

          if (((xs != xp && ys != yp) || (ys == yp)) && disambiguity[0] != row[xs]) {
            strncat(disambiguity, &row[xs], 1);
          } else if (xs == xp && disambiguity[1] != column[ys]) continue;
        }
      }

      // Second time see apply the column if needed
      for (size_t i = 0; i < ps.count; i++) {
        ChessSquare *possible_piece = ps.items[i];
        if (possible_piece != NULL) {
          ptrdiff_t p_index = possible_piece - &chess_board.squares[0][0];
          int yp = p_index / NS;
          int xp = p_index % NS;

          if (!chess_board.board_flipped) yp = NS - yp - 1;
          else xp = NS - xp - 1;

          if (((xs != xp && ys != yp) || (ys == yp)) && disambiguity[0] != row[xs]) {
            strncat(disambiguity, &row[xs], 1);
          } else if (xs == xp && disambiguity[1] != column[ys]) {
            strncat(disambiguity, &column[ys], 1);
          }
        }
      }
      ut_da_free(&ps);
      strcat(move, disambiguity);
    }
    if (!chess_board.state.captured) {
      strncat(move, &row[xd], 1);
      strncat(move, &column[yd], 1);
    } else {
      if (chess_board.moving.src_piece.type == PAWN) strncat(move, &row[xs], 1);
      strcat(move, "x");
      strncat(move, &row[xd], 1);
      strncat(move, &column[yd], 1);
    }

    if (chess_board.moving.src_piece.type == PAWN &&
        !chess_board.state.promote && chess_board.state.promotion_done && chess_board.promotion_square != NULL) {

      char* piece_notation = get_piece_notation(chess_board.promotion_square->piece);
      char upper_piece_notation = toupper((unsigned char) piece_notation[0]);
      strcat(move, "=");
      size_t len = strlen(move);
      move[len] = upper_piece_notation;
      move[len+1] = '\0';
    }
    if (in_check(chess_board.color_turn)) {
      if (chess_board.result == CHECKMATE) strcat(move, "#");
      else strcat(move, "+");
    }
  }

  strcat(pgn, move);
  printf("%s", pgn);
  fflush(stdout);
}
