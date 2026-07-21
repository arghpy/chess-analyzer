#include "protocols/uci.h"
#include "protocols/common.h"
#include "defines.h"
#include "init.h"
#include "core.h"
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void generate_uci_move(char* dest)
{
  char *row    = "abcdefgh";
  char *column = "12345678";

  ptrdiff_t s_index = moving_piece.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = moving_piece.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (!chess_board.board_flipped) {
    ys = NS - ys - 1;
    yd = NS - yd - 1;
  } else {
    xs = NS - xs - 1;
    xd = NS - xd - 1;
  }

  if (game_state == PROMOTING)
    sprintf(dest, "%c%c%c%c%c", row[xs], column[ys], row[xd], column[yd], tolower((unsigned char) get_piece_notation(moving_piece.dest->piece)[0]));
  else
    sprintf(dest, "%c%c%c%c", row[xs], column[ys], row[xd], column[yd]);
}
