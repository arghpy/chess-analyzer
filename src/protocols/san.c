#include "protocols/san.h"
#include "protocols/common.h"
#include "init.h"
#include "rules/general.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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
  if (chess_board.moving.src_piece.type != PAWN) {
    char* piece_notation = get_piece_notation(chess_board.moving.src_piece);
    char upper_piece_notation = toupper((unsigned char) piece_notation[0]);
    size_t len = strlen(move);
    move[len] = upper_piece_notation;
    move[len+1] = '\0';
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

  strcat(pgn, move);
  printf("%s", pgn);
  fflush(stdout);
}
