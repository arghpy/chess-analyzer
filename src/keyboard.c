#include "keyboard.h"
#include "protocols/fen.h"
#include "raylib.h"
#include "init.h"
#include "render.h"

void process_keyboard_events(void)
{
  if (IsKeyPressed(KEY_SPACE)) {
    chess_board.board_flipped = !chess_board.board_flipped;
    flip_board();
  }
  if(IsKeyPressed(KEY_P)) iterate_fen_positions();
}
