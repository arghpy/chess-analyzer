#include "rules/pieces.h"
#include "render.h"
#include "core.h"
#include "rules/general.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

bool knight_is_legal_move(const ChessSquare *src, const ChessSquare *dest)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  if ((dx == SQUARE_SIZE && dy == 2*SQUARE_SIZE) || (dx == 2*SQUARE_SIZE && dy == SQUARE_SIZE))
    return true;
  else return false;
}

bool bishop_is_legal_move(const ChessSquare *src, const ChessSquare *dest)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  int x_step = (dest->rect.x - src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (dest->rect.y - src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (dx == dy) {
    ys += y_step;
    xs += x_step;
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;
      ys += y_step;
      xs += x_step;
    }

    return true;
  } else return false;
}

bool rook_is_legal_move(const ChessSquare *src, const ChessSquare *dest, const ChessPiece src_piece)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  int x_step = (dest->rect.x - src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (dest->rect.y - src->rect.y) < 0 ?
                -1 : 1;

  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  if (dx == 0 || dy == 0) {
    if (ys == yd) {
      xs += x_step;
      while (xs != xd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        xs += x_step;
      }
    } else if (xs == xd) {
      ys += y_step;
      while (ys != yd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        ys += y_step;
      }
    }
    // Prevent castling if it moved from src:
    //  - long side:  0,0 || 7,0
    //  - short side: 0,7 || 7,7
    // TODO: part of the most awful pieces of code. CHANGE
    if (!chess_board.board_flipped) {
      if ((ys == 0 && xs == 0) || (ys == (NS - 1) && xs == 0)) {
        if (src_piece.color == W) chess_board.castle.w_l_can_castle = false;
        else chess_board.castle.b_l_can_castle = false;
      } else if ((ys == 0 && xs == (NS - 1)) || (ys == (NS - 1) && xs == (NS - 1))) {
        if (src_piece.color == W) chess_board.castle.w_s_can_castle = false;
        else chess_board.castle.b_s_can_castle = false;
      }
    } else {
      if ((ys == 0 && xs == 0) || (ys == (NS - 1) && xs == 0)) {
        if (src_piece.color == W) chess_board.castle.w_s_can_castle = false;
        else chess_board.castle.b_s_can_castle = false;
      } else if ((ys == 0 && xs == (NS - 1)) || (ys == (NS - 1) && xs == (NS - 1))) {
        if (src_piece.color == W) chess_board.castle.w_l_can_castle = false;
        else chess_board.castle.b_l_can_castle = false;
      }
    }

    return true;
  } else return false;
}

bool queen_is_legal_move(const ChessSquare *src, const ChessSquare *dest)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  int x_step = (dest->rect.x - src->rect.x) < 0 ?
    -1 : 1;
  int y_step = (dest->rect.y - src->rect.y) < 0 ?
    -1 : 1;

  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  // Check for rook move
  if (dx == 0 || dy == 0) {
    if (ys == yd) {
      xs += x_step;
      while (xs != xd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        xs += x_step;
      }
    } else if (xs == xd) {
      ys += y_step;
      while (ys != yd) {
        if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
          return false;
        ys += y_step;
      }
    }
    return true;
    // Check for bishop move
  } else if (dx == dy) {
    ys += y_step;
    xs += x_step;
    while (ys != yd || xs != xd) {
      if (chess_board.squares[ys][xs].piece.type != NO_PIECE)
        return false;

      ys += y_step;
      xs += x_step;
    }

    return true;
  } else return false;
}

bool king_is_legal_move(const ChessSquare *src, const ChessSquare *dest, const ChessPiece src_piece)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  int x_step = (dest->rect.x - src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (dest->rect.y - src->rect.y) < 0 ?
                -1 : 1;

  (void)y_step;

  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  // General movement
  if (((dx == 0 || dy == 0) && (abs(xs - xd) == 1 || abs(ys - yd) == 1)) ||
      ((dx == dy) && (abs(xs - xd) == 1 || abs(ys - yd) == 1))) {

    // Touching kings
    int lcx = (xd - 1) >= 0 ? xd - 1 : xd;
    int lcy = (yd - 1) >= 0 ? yd - 1 : yd;

    int rcx = (xd + 1) <= (NS - 1) ? xd + 1 : xd;
    int rcy = (yd + 1) <= (NS - 1) ? yd + 1 : yd;

    for (int y = lcy; y <= rcy; y++) {
      for (int x = lcx; x <= rcx; x++) {
        if (x == xs && y == ys) continue;
        if (chess_board.squares[y][x].piece.type == KING)
          return false;
      }
    }
    // Cannot castle anymore
    if (src_piece.color == W) {
      chess_board.castle.w_s_can_castle = false;
      chess_board.castle.w_l_can_castle = false;
    }
    if (src_piece.color == B) {
      chess_board.castle.b_s_can_castle = false;
      chess_board.castle.b_l_can_castle = false;
    }

    return true;
  } else if (dy == 0 && abs(xs - xd) == 2) {
    // TODO: part of the most awful pieces of code. CHANGE
    if (x_step > 0) {
      if (!chess_board.board_flipped) {
        if ((src_piece.color == W && chess_board.castle.w_s_can_castle) ||
            (src_piece.color == B && chess_board.castle.b_s_can_castle)) {
          int x = xs;
          while (x != (NS - 1)) {
            if (chess_board.squares[yd][x].piece.type != NO_PIECE) return false;
            else {
              chess_board.squares[yd][x].piece = src_piece;
              if (in_check(src_piece.color)) {
                reset_chess_square(&chess_board.squares[yd][x]);
                return false;
              } else reset_chess_square(&chess_board.squares[yd][x]);
            }
            x += x_step;
          }
          if (src_piece.color == W) {
            if (chess_board.squares[7][7].piece.color != W) return false;
            if (chess_board.squares[7][7].piece.type != ROOK) return false;
            chess_board.squares[7][5].piece.type  = ROOK;
            chess_board.squares[7][5].piece.color = W;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[7][7]);
            chess_board.castle.w_l_can_castle = false;
            chess_board.castle.w_s_can_castle = false;
          } else {
            if (chess_board.squares[0][7].piece.color != B) return false;
            if (chess_board.squares[0][7].piece.type != ROOK) return false;
            chess_board.squares[0][5].piece.type  = ROOK;
            chess_board.squares[0][5].piece.color = B;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[0][7]);
            chess_board.castle.b_l_can_castle = false;
            chess_board.castle.b_s_can_castle = false;
          }
        } else return false;
      } else {
        if ((src_piece.color == W && chess_board.castle.w_l_can_castle) ||
            (src_piece.color == B && chess_board.castle.b_l_can_castle)) {
          int x = xs;
                      if (!chess_board.state.verify) while (x != (NS - 1)) {
            if (chess_board.squares[yd][x].piece.type != NO_PIECE) return false;
            else {
              chess_board.squares[yd][x].piece = src_piece;
              if (in_check(src_piece.color)) {
                reset_chess_square(&chess_board.squares[yd][x]);
                return false;
              } else reset_chess_square(&chess_board.squares[yd][x]);
            }
            x += x_step;
          }
          if (src_piece.color == W) {
            if (chess_board.squares[0][7].piece.color != W) return false;
            if (chess_board.squares[0][7].piece.type != ROOK) return false;
            chess_board.squares[0][4].piece.type  = ROOK;
            chess_board.squares[0][4].piece.color = W;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[0][7]);
            chess_board.castle.w_l_can_castle = false;
            chess_board.castle.w_s_can_castle = false;
          } else {
            if (chess_board.squares[7][7].piece.color != B) return false;
            if (chess_board.squares[7][7].piece.type != ROOK) return false;
            chess_board.squares[7][4].piece.type  = ROOK;
            chess_board.squares[7][4].piece.color = B;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[7][7]);
            chess_board.castle.b_l_can_castle = false;
            chess_board.castle.b_s_can_castle = false;
          }
        } else return false;
      }
    } else {
      if (!chess_board.board_flipped) {
        if ((src_piece.color == W && chess_board.castle.w_l_can_castle) ||
            (src_piece.color == B && chess_board.castle.b_l_can_castle)) {
          int x = xs;
          while (x != (NS - 1)) {
            if (chess_board.squares[yd][x].piece.type != NO_PIECE) return false;
            else {
              chess_board.squares[yd][x].piece = src_piece;
              if (in_check(src_piece.color)) {
                reset_chess_square(&chess_board.squares[yd][x]);
                return false;
              } else reset_chess_square(&chess_board.squares[yd][x]);
            }
            x += x_step;
          }
          if (src_piece.color == W) {
            if (chess_board.squares[7][0].piece.color != W) return false;
            if (chess_board.squares[7][0].piece.type != ROOK) return false;
            chess_board.squares[7][3].piece.type  = ROOK;
            chess_board.squares[7][3].piece.color = W;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[7][0]);
            chess_board.castle.w_l_can_castle = false;
            chess_board.castle.w_s_can_castle = false;
          } else {
            if (chess_board.squares[0][0].piece.color != B) return false;
            if (chess_board.squares[0][0].piece.type != ROOK) return false;
            chess_board.squares[0][3].piece.type  = ROOK;
            chess_board.squares[0][3].piece.color = B;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[0][0]);
            chess_board.castle.b_l_can_castle = false;
            chess_board.castle.b_s_can_castle = false;
          }
        } else return false;
      } else {
        if ((src_piece.color == W && chess_board.castle.w_s_can_castle) ||
            (src_piece.color == B && chess_board.castle.b_s_can_castle)) {
          int x = xs;
          while (x != (NS - 1)) {
            if (chess_board.squares[yd][x].piece.type != NO_PIECE) return false;
            else {
              chess_board.squares[yd][x].piece = src_piece;
              if (in_check(src_piece.color)) {
                reset_chess_square(&chess_board.squares[yd][x]);
                return false;
              } else reset_chess_square(&chess_board.squares[yd][x]);
            }
            x += x_step;
          }
          if (src_piece.color == W) {
            if (chess_board.squares[0][0].piece.color != W) return false;
            if (chess_board.squares[0][0].piece.type != ROOK) return false;
            chess_board.squares[0][2].piece.type  = ROOK;
            chess_board.squares[0][2].piece.color = W;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[0][0]);
            chess_board.castle.w_l_can_castle = false;
            chess_board.castle.w_s_can_castle = false;
          } else {
            if (chess_board.squares[7][0].piece.color != B) return false;
            if (chess_board.squares[7][0].piece.type != ROOK) return false;
            chess_board.squares[7][2].piece.type  = ROOK;
            chess_board.squares[7][2].piece.color = B;
            if (!chess_board.state.verify) reset_chess_square(&chess_board.squares[7][0]);
            chess_board.castle.b_l_can_castle = false;
            chess_board.castle.b_s_can_castle = false;
          }
        } else return false;
      }
    }
    return true;
  } else return false;
}

void select_for_promotion(ChessSquare *promotion_square)
{
  for (int i = 0; i < NS; i++) {
      ChessSquare square = piece_promotions[i];
      if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
          (square.piece.type != NO_PIECE)) {

        chess_board.state.hovering_promotion = true;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          chess_board.state.hovering_piece = false;
          chess_board.state.promote = false;
          chess_board.state.promotion_done = true;
          promotion_square->piece = piece_promotions[i].piece;
          break;
        }
      }
  }
}

void promote_pawn(ChessSquare *promotion_square)
{
  ptrdiff_t d_index = promotion_square - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  int y_step = (yd == (NS-1)) ? 1 : -1;

  for (int i = 0; i < 4; i++) {
    piece_promotions[i].rect = (Rectangle) {
      .x      = xd * SQUARE_SIZE,
      .y      = (yd - y_step * i) * SQUARE_SIZE,
      .width  = SQUARE_SIZE,
      .height = SQUARE_SIZE,
    };
    piece_promotions[i].center_proximity = (Circle) {
      .center = (Vector2) {
        .x = xd * SQUARE_SIZE + SQUARE_SIZE / 2,
        .y = (yd - y_step * i) * SQUARE_SIZE + SQUARE_SIZE / 2
      },
        .r = SQUARE_SIZE / 2.5f,
    };
    piece_promotions[i].piece.color = promotion_square->piece.color == W ? W : B;
    DrawRectangleRec(piece_promotions[i].rect, RAYWHITE);
    DrawRectangleLinesEx(piece_promotions[i].rect, 2.0f, GRAY);
    draw_piece(&piece_promotions[i]);
  }
  select_for_promotion(promotion_square);
}

bool pawn_is_legal_move(const ChessSquare *src, ChessSquare *dest, const ChessPiece src_piece)
{
  float dx = fabsf(dest->rect.x - src->rect.x);
  float dy = fabsf(dest->rect.y - src->rect.y);

  int x_step = (dest->rect.x - src->rect.x) < 0 ?
                -1 : 1;
  int y_step = (dest->rect.y - src->rect.y) < 0 ?
                -1 : 1;
  (void)x_step;

  ptrdiff_t s_index = src - &chess_board.squares[0][0];
  int ys = s_index / NS;
  int xs = s_index % NS;

  ptrdiff_t d_index = dest - &chess_board.squares[0][0];
  int yd = d_index / NS;
  int xd = d_index % NS;

  // Allow it to only go forward
  if (!chess_board.board_flipped) {
    if (src_piece.color == W && y_step > 0) return false;
    if (src_piece.color == B && y_step < 0) return false;
  } else {
    if (src_piece.color == W && y_step < 0) return false;
    if (src_piece.color == B && y_step > 0) return false;
  }

  if (dx == 0) {
    bool allow_2_squares = false;

    // 2 move square allowed
    if (ys == 1 || ys == 6) allow_2_squares = true;

    if (dy == 2*SQUARE_SIZE) {
      if (!allow_2_squares) return false;
      int y = ys;
      while (y != yd) {
        if (chess_board.squares[y][xd].piece.type != NO_PIECE)
          return false;
        y += y_step;
      }

      // En passant
      if (chess_board.squares[yd][xd - 1].piece.type == PAWN &&
          chess_board.squares[yd][xd - 1].piece.color != src_piece.color) {
        chess_board.enpassant.allowed = true;
        chess_board.enpassant.allowed_by_color = src_piece.color;
        chess_board.enpassant.square = &chess_board.squares[yd - y_step][xd];
      }

      if (chess_board.squares[yd][xd + 1].piece.type == PAWN &&
          chess_board.squares[yd][xd + 1].piece.color != src_piece.color) {
        chess_board.enpassant.allowed = true;
        chess_board.enpassant.allowed_by_color = src_piece.color;
        chess_board.enpassant.square = &chess_board.squares[yd - y_step][xd];
      }

      return true;
    }

    if (dy != SQUARE_SIZE) return false;
    if (chess_board.squares[yd][xd].piece.type != NO_PIECE) return false;

    return true;
  } else if ((dx == dy) && (abs(xs - xd) == 1 || abs(ys - yd) == 1)) {
    if (!chess_board.enpassant.allowed) {
      if (chess_board.squares[yd][xd].piece.type == NO_PIECE) return false;
      else return true;
    } else {
      if (dest == chess_board.enpassant.square) {
        if (!chess_board.state.verify) {
          chess_board.moving.captured_piece = chess_board.squares[yd - y_step][xd].piece;
          reset_chess_square(&chess_board.squares[yd - y_step][xd]);
          chess_board.enpassant.done = true;
          chess_board.enpassant.allowed = false;
          chess_board.enpassant.square = NULL;
        }
        return true;
      } else return false;
    }
  } else return false;
}
