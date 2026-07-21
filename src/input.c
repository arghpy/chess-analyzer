#include "core.h"
#include "globals.h"
#include "init.h"
#include "input.h"
#include "protocols/fen.h"
#include "protocols/san.h"
#include "raylib.h"
#include "raymath.h"
#include "render.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>

typedef struct {
  int key;
  bool pressing;
  struct timeval start;
  struct timeval end;
} KeyPressed;

float get_time_key_pressed(KeyPressed* key_pressed)
{
  float result = 0;
  if (IsKeyDown(key_pressed->key) || key_pressed->pressing) {
    if (key_pressed->pressing == false) gettimeofday(&key_pressed->start, NULL);
    key_pressed->pressing = true;

    gettimeofday(&key_pressed->end, NULL);
    result = (key_pressed->end.tv_sec - key_pressed->start.tv_sec) * 1000000 + (key_pressed->end.tv_usec - key_pressed->start.tv_usec);

    if (IsKeyReleased(key_pressed->key)) {
      key_pressed->pressing = false;
      key_pressed->start    = (struct timeval){0};
      key_pressed->end      = (struct timeval){0};
    }
  }
  result /= 1000000.0; // Seconds
  return result; // Seconds
}

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
  static KeyPressed key_left_pressed = {0};
  key_left_pressed.key = KEY_LEFT;
  if (IsKeyPressed(KEY_LEFT) || (get_time_key_pressed(&key_left_pressed) > 0.3 && frames_passed % 10 == 0)) {
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
        load_fen(ll_chess_move_current->value.fen);
        chess_board.action_sound = ll_chess_move_current->value.sound;
      }
    }
  }
  static KeyPressed key_right_pressed = {0};
  key_right_pressed.key = KEY_RIGHT;
  if (IsKeyPressed(KEY_RIGHT) || (get_time_key_pressed(&key_right_pressed) > 0.3 && frames_passed % 10 == 0)) {
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
        load_fen(ll_chess_move_current->value.fen);
        chess_board.action_sound = ll_chess_move_current->value.sound;
      }
    }
  }
}

void process_mouse_right_button(void)
{
  static SrcDestChessSquares mouse_right_button_square = {0};

  // Color square red on right mouse click
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect)) {
          mouse_right_button_square.src = s;
          break;
        }
      }
      if (mouse_right_button_square.src != NULL) break;
    }
  }
  if (mouse_right_button_square.src != NULL && IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect)) mouse_right_button_square.dest = s;
      }
    }
  }

  if (mouse_right_button_square.src != NULL && mouse_right_button_square.dest != NULL) {
    if (mouse_right_button_square.src == mouse_right_button_square.dest) {
      ptrdiff_t index = mouse_right_button_square.src - &chess_board.squares[0][0];
      int y = index / NS;
      int x = index % NS;

      if (!ColorIsEqual(mouse_right_button_square.src->board_color, red_square_color[(x + y) % 2])) mouse_right_button_square.src->board_color = red_square_color[(x + y) % 2];
      else {
        reset_square_color(mouse_right_button_square.src);
        if (mouse_right_button_square.src == ll_chess_move_tail->value.src || mouse_right_button_square.src == ll_chess_move_tail->value.dest)
          mouse_right_button_square.src->board_color = color_occupied_square(mouse_right_button_square.src);
      }
    } else add_arrow(mouse_right_button_square.src, mouse_right_button_square.dest);
    mouse_right_button_square.src = NULL;
    mouse_right_button_square.dest = NULL;
  }

  for (size_t i = 0; i < drawn_arrows.count; i++)
    draw_arrow(&(drawn_arrows.items[i]));
}

void process_mouse_left_button(void)
{
  bool found = false;

  // Color square "as-moved" on left mouse click
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    // Remove drawn arrows
    drawn_arrows.count = 0;

    for (int y = 0; y < NS; y++) {
      for (int x = 0; x < NS; x++) {
        ChessSquare *s = &chess_board.squares[y][x];
        if (CheckCollisionPointRec(GetMousePosition(), s->rect)) {
          if (s->piece.type == NO_PIECE) reset_colors_for_current_move();
          else {
            if (!ColorIsEqual(s->board_color, occupied_square_color[LIGHT_TILE]) &&
                !ColorIsEqual(s->board_color, occupied_square_color[DARK_TILE]))
            {
              reset_colors_for_current_move();
              s->board_color = color_occupied_square(s);
            }
            found = true;
            break;
          }
        }
      }
      if (found) break;
    }
  }
}

void process_mouse_events(void)
{
  process_mouse_right_button();
  process_mouse_left_button();
}
