#include "core.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "protocols/uci.h"
#include "render.h"
#include "raylib.h"
#include "rules/general.h"
#include "rules/pieces.h"
#include "input.h"
#include <stddef.h>
#include <stdio.h>

GameState game_state = PLAYING;
MenuState menu_state = MAIN;
bool found_placement_square = false;

bool same_chess_piece(ChessPiece a, ChessPiece b)
{
  return (a.type == b.type && a.color == b.color);
}

void process_menu_states(const Font* general_font, const Font* big_font)
{
  ClearBackground(background_color);
  draw_chess_board(general_font);
  process_mouse_events();

  switch (menu_state) {
    case MAIN:
      draw_title(general_font);
      draw_paste_fen_window(general_font);
      draw_paste_pgn_window(general_font);
      draw_import_fen_pgn_button(general_font);
      draw_moving_piece();
      if (chess_board.state.piece_placed) {
        menu_state = ANALYSIS;
        verify_if_any_legal_move(chess_board.color_turn);

        // Full moves
        if (ll_chess_move_current->prev != NULL && moving_piece.piece.color == W)
          chess_board.fullmoves += 1;

        moving_piece.sound   = chess_board.action_sound;
        moving_piece.move_nr = chess_board.fullmoves;
        generate_fen(moving_piece.fen);
        generate_san(moving_piece.san);
        generate_uci_move(moving_piece.uci_move);
        ut_ll_push(ChessMoveNode, ll_chess_move_head, moving_piece, ll_chess_move_tail);
        ll_chess_move_current = ll_chess_move_tail;
        advance_game_parameters();
      }
      break;
    case ANALYSIS:
      process_keyboard_events();
      // Needs to be first such that writing can be displayed
      // even if it's only half displayed on screen
      draw_san_window(general_font);
      draw_title(general_font);
      draw_copy_fen_button(general_font);
      draw_copy_pgn_button(general_font);
      process_game_states(big_font);
      break;
  }
}

void advance_game_parameters(void)
{
  chess_board.state.piece_placed = false;

  if (chess_board.enpassant.done) chess_board.enpassant.done = false;

  // Half moves
  if (moving_piece.piece.type == PAWN || chess_board.state.captured)
    chess_board.halfmoves = 0;
  else chess_board.halfmoves += 1;

  if (chess_board.halfmoves == 50) {
    game_state = DRAW;
  }

  // Castling
  chess_board.castle.castled = NO;

  // Captured piece
  if (chess_board.state.captured) {
    chess_board.state.captured = false;
    chess_board.captured_piece = (ChessPiece){0};
  }
  if (game_state != DRAW && game_state != CHECKMATE) game_state = PLAYING;
}

void process_game_states(const Font* font)
{
  // Allow moves to continue only from the end
  if (game_state == REWINDING && ll_chess_move_current == ll_chess_move_tail)
    game_state = PLAYING;

  switch (game_state) {
    case REWINDING: break;
    case PLAYING:
      draw_moving_piece();
      if (chess_board.state.piece_placed && game_state != PROMOTING) {
        verify_if_any_legal_move(chess_board.color_turn);

        // Full moves
        if (ll_chess_move_current->prev != NULL && moving_piece.piece.color == W)
          chess_board.fullmoves += 1;

        moving_piece.sound   = chess_board.action_sound;
        moving_piece.move_nr = chess_board.fullmoves;
        generate_fen(moving_piece.fen);
        generate_san(moving_piece.san);
        generate_uci_move(moving_piece.uci_move);
        ut_ll_push(ChessMoveNode, ll_chess_move_head, moving_piece, ll_chess_move_tail);
        ll_chess_move_current = ll_chess_move_tail;
        advance_game_parameters();
      }
      break;
    case PROMOTING:
      draw_promotion_pieces();
      ChessPiece promotion_piece = select_for_promotion();
      if (promotion_piece.type != NO_PIECE) {
        moving_piece.dest->piece = promotion_piece;
        verify_if_any_legal_move(chess_board.color_turn);

        // Full moves
        if (ll_chess_move_current->prev != NULL && moving_piece.dest->piece.color == W)
          chess_board.fullmoves += 1;

        moving_piece.sound   = chess_board.action_sound;
        moving_piece.move_nr = chess_board.fullmoves;
        generate_fen(moving_piece.fen);
        generate_san(moving_piece.san);
        generate_uci_move(moving_piece.uci_move);
        ut_ll_push(ChessMoveNode, ll_chess_move_head, moving_piece, ll_chess_move_tail);
        ll_chess_move_current = ll_chess_move_tail;

        advance_game_parameters();
      }
      break;
    case DRAW:
      draw_result(font, "DRAW");
      break;
    case CHECKMATE:
      draw_result(font, chess_board.color_turn == W ? "BLACK WON" : "WHITE WON");
      break;
  }
}

void change_chess_board_turn(void)
{
  if (chess_board.color_turn == W) chess_board.color_turn = B;
  else chess_board.color_turn = W;
}

void reset_square_color(ChessSquare *s)
{
  ptrdiff_t p_index = s - &chess_board.squares[0][0];
  int y = p_index / NS;
  int x = p_index % NS;

  s->board_color = square_color[(x + y) % 2];
}

void reset_square_color_chess_move(ChessMoveNode *node)
{
  if (node != NULL && node->value.src != NULL && node->value.dest != NULL) {
    ptrdiff_t p_s_index = node->value.src - &chess_board.squares[0][0];
    int ys = p_s_index / NS;
    int xs = p_s_index % NS;

    node->value.src->board_color = square_color[(xs + ys) % 2];

    ptrdiff_t p_d_index = node->value.dest - &chess_board.squares[0][0];
    int yd = p_d_index / NS;
    int xd = p_d_index % NS;

    node->value.dest->board_color = square_color[(xd + yd) % 2];
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
        if (!valid_move(moving_piece.src, square_ptr, moving_piece.piece)) {
          // This additional check is only for setting the sound
          if (still_on_src_square(moving_piece.src, square_ptr)) chess_board.action_sound = NOTHING;
          else chess_board.action_sound = ILLEGAL;
          break;
        } else {
          // Needed for verifying if current player is in check
          square_ptr->piece = moving_piece.piece;

          if (in_check(moving_piece.piece.color)) {
            chess_board.action_sound = ILLEGAL;
            square_ptr->piece = square_piece_copy;
            break;
          }

          // Sounds:
          if (square_piece_copy.type != NO_PIECE || chess_board.enpassant.done) {
            chess_board.state.captured = true;
            chess_board.captured_piece = square_piece_copy;
            chess_board.action_sound = CAPTURE;
          } else if (chess_board.castle.castled) {
            chess_board.action_sound = CASTLE;
          } else chess_board.action_sound = MOVE;

          chess_board.state.piece_placed = true;
          moving_piece.dest = square_ptr;

          // Reset original colors and set colors for new valid moves
          reset_square_color_chess_move(ll_chess_move_tail);
          reset_square_color(moving_piece.src);
          reset_square_color(moving_piece.dest);

          moving_piece.src->board_color  = color_occupied_square(moving_piece.src);
          moving_piece.dest->board_color = color_occupied_square(moving_piece.dest);

          break;
        }
        break;
      }
    }
    if (found_placement_square) break;
  }
  found_placement_square = false;
  if (!chess_board.state.piece_placed)
    moving_piece.src->piece = moving_piece.piece;
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}
