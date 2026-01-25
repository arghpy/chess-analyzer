#include "rules/pieces.h"
#include "rules/general.h"
#include "board.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

bool knight_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  if (still_on_src_square(dx, dy)) return false;
  if ((dx == SQUARE_SIZE && dy == 2*SQUARE_SIZE) || (dx == 2*SQUARE_SIZE && dy == SQUARE_SIZE))
    return true;
  else return false;
}

bool bishop_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int x_step = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (still_on_src_square(dx, dy)) return false;

  if (dx == dy) {
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;

      ys += y_step;
      xs += x_step;
    }

    return true;
  } else return false;
}

bool rook_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int x_step = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (still_on_src_square(dx, dy)) return false;

  if (dx == 0 || dy == 0) {
    if (ys == yd) {
      while (xs != xd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        xs += x_step;
      }
    } else if (xs == xd) {
      while (ys != yd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        ys += y_step;
      }
    }
    return true;
  } else return false;
}

bool queen_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int x_step = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
    -1 : 1;
  int y_step = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
    -1 : 1;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (still_on_src_square(dx, dy)) return false;

  // Check for rook move
  if (dx == 0 || dy == 0) {
    if (ys == yd) {
      while (xs != xd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        xs += x_step;
      }
    } else if (xs == xd) {
      while (ys != yd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        ys += y_step;
      }
    }
    return true;
    // Check for bishop move
  } else if (dx == dy) {
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;

      ys += y_step;
      xs += x_step;
    }

    return true;
  } else return false;
}

bool king_is_legal_move(void)
{
  float dx = fabsf(chess_board.dest->rect.x - chess_board.src->rect.x);
  float dy = fabsf(chess_board.dest->rect.y - chess_board.src->rect.y);

  int x_step = (chess_board.dest->rect.x - chess_board.src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (chess_board.dest->rect.y - chess_board.src->rect.y) < 0 ?
                -1 : 1;

  (void)x_step;
  (void)y_step;

  ptrdiff_t s_index = chess_board.src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = chess_board.dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (still_on_src_square(dx, dy)) return false;

  // General movement
  if (((dx == 0 || dy == 0) && (abs(xs - xd) == 1 || abs(ys - yd) == 1)) ||
      ((dx == dy) && (abs(xs - xd) == 1 || abs(ys - yd) == 1))) {

    // Touching kings
    int lcx = (xd - 1) >= 0 ? xd - 1 : xd;
    int lcy = (yd - 1) >= 0 ? yd - 1 : yd;

    int rcx = (xd + 1) <= (NS - 1) ? xd + 1 : xd;
    int rcy = (yd + 1) <= (NS - 1) ? yd + 1 : yd;

    for (int x = lcx; x <= rcx; x++) {
      for (int y = lcy; y <= rcy; y++) {
        if (x == xd && y == yd) continue;
        if (chess_board.squares[y][x].piece.type == KING)
          return false;
      }
    }
    return true;
  // } else if (dy == 0 && abs(xs - xd) == 2) {
  //   if ((chess_board.src_piece.color == W && !chess_board.w_castled) ||
  //       (chess_board.src_piece.color == B && !chess_board.b_castled)) {
  //     int x = xs;
  //     while (x != xd) {
  //       if (chess_board.squares[yd][x].piece.type != NO_PIECE)
  //         return false;
  //       x += x_step;
  //     }
  //   }
  } else return false;
}
