#include "core.h"
#include "render.h"
#include "raylib.h"
#include "rules/general.h"
#include <stddef.h>
#include <stdio.h>

void increment_game_states(void)
{
  if (chess_board.enpassant.done) chess_board.enpassant.done = false;

  // Record which color piece moved
  if      (chess_board.moving.src_piece.color == W) chess_board.state.w_moved = true;
  else if (chess_board.moving.src_piece.color == B) chess_board.state.b_moved = true;

  // Full moves
  if (chess_board.state.w_moved && chess_board.state.b_moved) {
    chess_board.fullmoves += 1;
    chess_board.state.w_moved = false;
    chess_board.state.b_moved = false;
  }

  // Half moves
  if (chess_board.moving.src_piece.type == PAWN || chess_board.state.captured)
    chess_board.halfmoves = 0;
  else chess_board.halfmoves += 1;

  if (chess_board.halfmoves == 50) chess_board.result = DRAW;
}

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

void reset_color_previously_moved_pieces()
{
  if (chess_board.moving.p_src != NULL) {
    ptrdiff_t p_s_index = chess_board.moving.p_src - &chess_board.squares[0][0];
    int ys = p_s_index / NS;
    int xs = p_s_index % NS;

    chess_board.moving.p_src->board_color = square_color[(xs + ys) % 2];
  }

  if (chess_board.moving.p_dest != NULL) {
    ptrdiff_t p_d_index = chess_board.moving.p_dest - &chess_board.squares[0][0];
    int yd = p_d_index / NS;
    int xd = p_d_index % NS;

    chess_board.moving.p_dest->board_color = square_color[(xd + yd) % 2];
  }
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square  = &chess_board.squares[y][x];
      ChessPiece square_p  = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(),
                                    square->center_proximity.center,
                                    square->center_proximity.r)) {
        if (!valid_move(chess_board.moving.c_src, square, chess_board.moving.src_piece)) {
          chess_board.moving.wrong_move = true;
          break;
        } else {
          square->piece = chess_board.moving.src_piece;
          if (in_check(chess_board.moving.src_piece.color)) {
            square->piece = square_p;
            chess_board.moving.wrong_move = true;
            break;
          }
          if (square_p.type != NO_PIECE && !chess_board.enpassant.done) {
            chess_board.state.captured = true;
            chess_board.moving.captured_piece = square_p;
          }

          // Reset original colors and set colors for new valid moves
          reset_color_previously_moved_pieces();
          chess_board.moving.p_src               = chess_board.moving.c_src;
          chess_board.moving.p_dest              = square;
          chess_board.moving.p_src->board_color  = color_occupied_square(chess_board.moving.p_src);
          chess_board.moving.p_dest->board_color = color_occupied_square(chess_board.moving.p_dest);

          change_chess_board_turn();
          chess_board.moving.c_dest = &chess_board.squares[y][x];
          chess_board.state.piece_placed = true;
          break;
        }
        if (chess_board.moving.wrong_move) break;
      }
    }
    if (chess_board.state.piece_placed) break;
    if (chess_board.moving.wrong_move) break;
  }
  if (!chess_board.state.piece_placed)
    chess_board.moving.c_src->piece = chess_board.moving.src_piece;
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}
