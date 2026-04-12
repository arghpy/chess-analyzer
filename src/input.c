#include "input.h"
#include "protocols/fen.h"
#include "raylib.h"
#include "init.h"
#include "render.h"
#include <stdio.h>

void process_keyboard_events(void)
{
  if (IsKeyPressed(KEY_SPACE)) {
    chess_board.board_flipped = !chess_board.board_flipped;
    flip_board();
  }
  if (IsKeyPressed(KEY_P)) {
    switch (game_state) {
      case PLAYING:
        printf("\n[Result \"*\"]\n\n");
        break;
      case PROMOTING:
        break;
      case DRAW:
        printf("\n[Result \"1/2-1/2\"]\n\n");
        break;
      case CHECKMATE: {
                        int white_result = chess_board.color_turn == W ? 0 : 1;
                        int black_result = chess_board.color_turn == W ? 1 : 0;
                        printf("\n[Result \"%d-%d\"]\n\n", white_result, black_result);
                        break;
                      }
    }
    for (size_t i = 0; i < san_moves.count; i++) {
      SanMove *full_move = &san_moves.items[i];
      printf("%d. %s %s\n", full_move->move_nr, full_move->san_w, full_move->san_b);
    }
  }
>>>>>>> ebcaa9c (fixup fen)
}
