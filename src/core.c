#include "core.h"
#include "fen.h"
#include "render.h"
#include "raylib.h"
#include "rules/pieces.h"
#include "rules/general.h"
#include <stddef.h>
#include <stdio.h>

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
        if (valid_move(chess_board.moving.c_src, square, chess_board.moving.src_piece)) {
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

          // Check promotion
          if (square->piece.type == PAWN) {
            ptrdiff_t d_index = square - &chess_board.squares[0][0];
            int yd = d_index / NS;

            if (yd == 0 || yd == (NS - 1)) {
              chess_board.state.promote = true;
              promote_pawn(chess_board.moving.c_src, square);
            }
          }

          change_chess_board_turn();
          chess_board.moving.c_dest = &chess_board.squares[y][x];
          chess_board.state.piece_placed = true;
          break;
        }
      }
    }
    if (chess_board.state.piece_placed) break;
    if (chess_board.moving.wrong_move) break;
  }
  if (!chess_board.state.piece_placed)
    chess_board.moving.c_src->piece = chess_board.moving.src_piece;
}

void draw_drag_and_place(void)
{
  if (!chess_board.state.dragging_piece) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *square = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), square->rect) &&
            (square->piece.type != NO_PIECE)
            ) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            chess_board.moving.src_piece = chess_board.squares[y][x].piece;
            chess_board.moving.c_src     = &chess_board.squares[y][x];
            chess_board.state.dragging_piece = true;
            reset_chess_square(square);
            break;
          }
        }
      }
      if (chess_board.state.dragging_piece) break;
    }
  }

  if (chess_board.state.dragging_piece) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle square = chess_board.moving.c_src->rect;
    square.x      = mouse_pos.x - SQUARE_SIZE / 2;
    square.y      = mouse_pos.y - SQUARE_SIZE / 2;

    ChessSquare moving_piece = {0};
    moving_piece.piece = chess_board.moving.src_piece;
    moving_piece.rect = square;
    draw_piece(&moving_piece);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      chess_board.state.dragging_piece = false;
      place_piece();
      if (chess_board.state.piece_placed) {
        generate_fen_position();
        verify_if_any_legal_move(chess_board.color_turn);
        if (chess_board.result == NONE) {
          if (chess_board.enpassant.done) chess_board.enpassant.done = false;

          // Record which color piece moved
          if      (chess_board.moving.src_piece.color == W) chess_board.state.w_moved = true;
          else if (chess_board.moving.src_piece.color == B) chess_board.state.b_moved = true;

          // Write PGN
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
              if (!chess_board.state.captured)
                snprintf(move, sizeof(pgn), " %c%c", row[xd], column[yd]);
              else
                snprintf(move, sizeof(pgn), " %cx%c%c", row[xs], row[xd], column[yd]);
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
        // Reset
        chess_board.moving.c_src = NULL;
        chess_board.moving.c_dest = NULL;
      }
    }
  }
}

void check_pieces_hovering(void)
{
  for (int y = 0; y < NS; y++) {
    for (int x = 0; x < NS; x++) {
      ChessSquare square = chess_board.squares[y][x];
      if (CheckCollisionPointRec(GetMousePosition(), square.rect) &&
          (square.piece.type != NO_PIECE)) {
        chess_board.state.hovering_piece = true;
        break;
      }
    }
    if (chess_board.state.hovering_piece) break;
  }
}

void set_mouse_cursor(void)
{
  SetMouseCursor(
      chess_board.state.hovering_promotion ||
      chess_board.state.hovering_piece ||
      chess_board.state.dragging_piece ?
      MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT
      );
}

void draw_moving_piece(void)
{
  if (!chess_board.state.promote) {
    check_pieces_hovering();
    draw_drag_and_place();
  }
}

void reset_chess_square(ChessSquare *square)
{
  square->piece.type = NO_PIECE;
  square->piece.color = N;
}
