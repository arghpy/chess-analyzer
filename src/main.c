#include <stdio.h>
#include "raylib.h"
#define background_color ((Color){0x18, 0x18, 0x18, 0xFF})

int main(void)
{
  InitWindow(800, 600, "Chess analyzer");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  while(!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(RED);
    }
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
