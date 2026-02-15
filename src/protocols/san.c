#include "protocols/san.h"
#include "protocols/common.h"
#include "init.h"
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
  char pgn[10] = {0};
  char move[10] = {0};

  if (chess_board.state.w_moved && !chess_board.state.b_moved)
    snprintf(pgn, sizeof(pgn), "\n%d.", chess_board.fullmoves);

  switch (chess_board.moving.src_piece.type) {
    case NO_PIECE:
    case PAWN:
      if (!chess_board.state.captured) {
        if (!chess_board.state.promote && chess_board.state.promotion_done) {
          if (chess_board.promotion_square != NULL) {
            char* piece_notation = get_piece_notation(chess_board.promotion_square->piece);
            char upper_piece_notation[2] = {0};
            for (size_t i = 0; i < strlen(piece_notation) && i < sizeof(upper_piece_notation); i++)
              upper_piece_notation[i] = toupper((unsigned char) piece_notation[i]);

            snprintf(move, sizeof(pgn), " %c%c=%s", row[xd], column[yd], upper_piece_notation);
          }
        } else
          snprintf(move, sizeof(pgn), " %c%c", row[xd], column[yd]);
      } else {
        if (!chess_board.state.promote && chess_board.state.promotion_done) {
          if (chess_board.promotion_square != NULL) {
            char* piece_notation = get_piece_notation(chess_board.promotion_square->piece);
            char upper_piece_notation[2] = {0};
            for (size_t i = 0; i < strlen(piece_notation) && i < sizeof(upper_piece_notation); i++)
              upper_piece_notation[i] = toupper((unsigned char) piece_notation[i]);

            snprintf(move, sizeof(pgn), " %cx%c%c=%s", row[xs], row[xd], column[yd], upper_piece_notation);
          }
        } else
          snprintf(move, sizeof(pgn), " %cx%c%c", row[xs], row[xd], column[yd]);
      }
    case BISHOP:
    case KING:
    case KNIGHT:
    case QUEEN:
    case ROOK:
    case PIECE_COUNT:
      break;
  }
  strcat(pgn, move);
  printf("%s", pgn);
  fflush(stdout);
}
