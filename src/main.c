#include "raylib.h"
#include "board.h"
#include "mouse.h"
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
      draw_mouse_cursor();
    }
    EndDrawing();
  }

  { // Closing everything
    unload_chess_pieces();
    UnloadFont(font);
    CloseWindow();
  }
  return 0;
}
