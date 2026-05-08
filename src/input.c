#include "input.h"
#include "core.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "init.h"
#include "render.h"
#include <stdio.h>
#include <stddef.h>

void process_keyboard_events(void)
{
  if (IsKeyPressed(KEY_SPACE)) {
    chess_board.board_flipped = !chess_board.board_flipped;
    flip_board();
  }

  if (IsKeyPressed(KEY_F)) {
    if (ll_chess_move_current != NULL) printf("|%s|\n", ll_chess_move_current->value.fen);
  }

  // Cycle through moves
  if (IsKeyPressed(KEY_LEFT)) {
    if (ll_chess_move_current != NULL) {
      if (ll_chess_move_current->prev != NULL) {
        game_state = REWINDING;

        // Reset current color
        reset_square_color_chess_move(ll_chess_move_current);

        // Assign new color
        if (ll_chess_move_current->prev->value.src != NULL && ll_chess_move_current->prev->value.dest != NULL) {
          ll_chess_move_current->prev->value.src->board_color  = color_occupied_square(ll_chess_move_current->prev->value.src);
          ll_chess_move_current->prev->value.dest->board_color = color_occupied_square(ll_chess_move_current->prev->value.dest);
        }

        ll_chess_move_current = ll_chess_move_current->prev;
        load_fen_position(ll_chess_move_current->value.fen);
        chess_board.action_sound = ll_chess_move_current->value.sound;
      }
    }
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    if (ll_chess_move_current != NULL) {
      if (ll_chess_move_current->next != NULL) {
        game_state = REWINDING;

        // Reset current color
        reset_square_color_chess_move(ll_chess_move_current);

        // Assign new color
        if (ll_chess_move_current->next->value.src != NULL && ll_chess_move_current->next->value.dest != NULL) {
          ll_chess_move_current->next->value.src->board_color  = color_occupied_square(ll_chess_move_current->next->value.src);
          ll_chess_move_current->next->value.dest->board_color = color_occupied_square(ll_chess_move_current->next->value.dest);
        }

        ll_chess_move_current = ll_chess_move_current->next;
        load_fen_position(ll_chess_move_current->value.fen);
        chess_board.action_sound = ll_chess_move_current->value.sound;
      }
    }
  }
}

void process_mouse_events(void)
{
  bool found = false;
  // Color square red on right mouse click
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect)) {
          if (!ColorIsEqual(s->board_color, RED_SQUARE)) s->board_color = RED_SQUARE;
          else {
            reset_square_color(s);
            if (s == ll_chess_move_tail->value.src || s == ll_chess_move_tail->value.dest)
              s->board_color = color_occupied_square(s);
          }
          found = true;
          break;
        }
      }
      if (found) break;
    }
  }
  // Color square "as-moved" on left mouse click
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect) && s->piece.type != NO_PIECE) {
          if (!ColorIsEqual(s->board_color, occupied_square_color[LIGHT_OCCUPIED_TILE]) &&
              !ColorIsEqual(s->board_color, occupied_square_color[DARK_OCCUPIED_TILE]))
          {
            for (int y = 0; y < NS; y++)
              for (int x = 0; x < NS; x++)
                reset_square_color(&chess_board.squares[y][x]);

            if (ll_chess_move_current->value.src != NULL && ll_chess_move_current->value.dest != NULL) {
              ll_chess_move_current->value.src->board_color  = color_occupied_square(ll_chess_move_current->value.src);
              ll_chess_move_current->value.dest->board_color = color_occupied_square(ll_chess_move_current->value.dest);
            }
            s->board_color = color_occupied_square(s);
          }
          found = true;
          break;
        }
      }
      if (found) break;
    }
  }
}
