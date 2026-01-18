#include "raylib.h"
#include "board.h"

#define background_color ((Color){0x18, 0x18, 0x18, 0xFF})

int main(void)
{
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(800, 600, "Chess analyzer");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  char *font_path = "./thirdparty/assets/fonts/JetBrainsMono-Bold.ttf";
  Font font = LoadFont(font_path);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

  load_chess_pieces();

  while(!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(background_color);
      draw_chess_board(&font);
    }
    EndDrawing();
  }

  // Closing everything
  {
    unload_chess_pieces();
    UnloadFont(font);
    CloseWindow();
  }
  return 0;
}
