#include "raylib.h"
#include "board.h"
#include "move.h"
#include "rules/pieces.h"
#include <stdbool.h>

#define background_color ((Color){0x18, 0x18, 0x18, 0xFF})

int main(void)
{
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 600, "Chess analyzer");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  char *font_path = "./assets/fonts/JetBrainsMono-Bold.ttf";
  Font font = LoadFont(font_path);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

  { // Initializing
    load_starting_position();
    load_chess_pieces();
    load_pawn_promotions();
  }

  while(!WindowShouldClose()) {
    if (IsKeyPressed(KEY_SPACE)) {
      chess_board.flipped = !chess_board.flipped;
      flip_board();
    }
    BeginDrawing();
    {
      ClearBackground(background_color);
      draw_chess_board(&font);
      draw_moving_piece();

      if (chess_board.promote) promote_pawn(chess_board.c_dest);

      set_mouse_cursor();
    }
    EndDrawing();

    // Reset
    chess_board.hovering_piece = false;
    chess_board.hovering_promotion = false;
  }

  { // Closing everything
    unload_chess_pieces();
    UnloadFont(font);
    CloseWindow();
  }
  return 0;
}
