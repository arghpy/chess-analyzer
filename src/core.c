#include "core.h"
#include "render.h"
#include "raylib.h"
#include "rules/general.h"
#include "rules/pieces.h"
#include <stddef.h>
#include <stdio.h>

GameState game_state = PLAYING;
bool found_placement_square = false;

void advance_game_parameters(void)
{
  chess_board.state.piece_placed = false;
  if (chess_board.state.w_moved && chess_board.state.b_moved) {
    chess_board.state.w_moved = false;
    chess_board.state.b_moved = false;
  }
  increment_chess_states();
}

void process_game_states(const Font* font)
{
  switch (game_state) {
    case PLAYING:
      draw_moving_piece();
      break;
    case PROMOTING:
      draw_promotion_pieces(chess_board.promotion_square);
      select_for_promotion(chess_board.promotion_square);
      break;
    case DRAW:
      draw_result(font, "DRAW");
      break;
    case CHECKMATE:
      draw_result(font, chess_board.color_turn == W ? "BLACK WON" : "WHITE WON");
      break;
  }
}

void increment_chess_states(void)
{
  if (chess_board.enpassant.done) chess_board.enpassant.done = false;

  // Record which color piece moved
  if      (chess_board.moving.src_piece.color == W) chess_board.state.w_moved = true;
  else if (chess_board.moving.src_piece.color == B) chess_board.state.b_moved = true;

  // Full moves
  if (chess_board.state.w_moved && chess_board.state.b_moved)
    chess_board.fullmoves += 1;

  // Half moves
  if (chess_board.moving.src_piece.type == PAWN || chess_board.state.captured)
    chess_board.halfmoves = 0;
  else chess_board.halfmoves += 1;

  if (chess_board.halfmoves == 50) game_state = DRAW;
}

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

void reset_color_previously_moved_pieces(void)
{
  if (chess_board.moving.prev_src != NULL) {
    ptrdiff_t p_s_index = chess_board.moving.prev_src - &chess_board.squares[0][0];
    int ys = p_s_index / NS;
    int xs = p_s_index % NS;

    chess_board.moving.prev_src->board_color = square_color[(xs + ys) % 2];
  }

  if (chess_board.moving.prev_dest != NULL) {
    ptrdiff_t p_d_index = chess_board.moving.prev_dest - &chess_board.squares[0][0];
    int yd = p_d_index / NS;
    int xd = p_d_index % NS;

    chess_board.moving.prev_dest->board_color = square_color[(xd + yd) % 2];
  }
}

void place_piece(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare *square_ptr      = &chess_board.squares[y][x];
      ChessPiece square_piece_copy = chess_board.squares[y][x].piece;
      if (CheckCollisionPointCircle(GetMousePosition(),
                                    square_ptr->center_proximity.center,
                                    square_ptr->center_proximity.r)) {
        found_placement_square = true;
        if (!valid_move(chess_board.moving.current_src, square_ptr, chess_board.moving.src_piece)) {
          // This additional check is only for setting the sound
          if (still_on_src_square(chess_board.moving.current_src, square_ptr)) chess_board.action_sound = NOTHING;
          else chess_board.action_sound = ILLEGAL;
          break;
        } else {
          // Needed for verifying if current player is in check
          square_ptr->piece = chess_board.moving.src_piece;
          if (in_check(chess_board.moving.src_piece.color)) {
            chess_board.action_sound = ILLEGAL;
            square_ptr->piece = square_piece_copy;
            break;
          }

          // Sounds:
          if (square_piece_copy.type != NO_PIECE || chess_board.enpassant.done) {
            chess_board.state.captured = true;
            chess_board.moving.captured_piece = square_piece_copy;
            chess_board.action_sound = CAPTURE;
          } else if (chess_board.castle.castled) {
            chess_board.action_sound = CASTLE;
          } else chess_board.action_sound = MOVE;

          chess_board.state.piece_placed = true;

          // Reset original colors and set colors for new valid moves
          reset_color_previously_moved_pieces();
          chess_board.moving.prev_src               = chess_board.moving.current_src;
          chess_board.moving.prev_dest              = square_ptr;
          chess_board.moving.prev_src->board_color  = color_occupied_square(chess_board.moving.prev_src);
          chess_board.moving.prev_dest->board_color = color_occupied_square(chess_board.moving.prev_dest);
          chess_board.moving.current_dest           = square_ptr;
          break;
        }
        break;
      }
    }
    if (found_placement_square) break;
  }
  found_placement_square = false;
  if (!chess_board.state.piece_placed)
    chess_board.moving.current_src->piece = chess_board.moving.src_piece;
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}
