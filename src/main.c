#include <stdio.h>
#include "raylib.h"

int main(void)
{
  InitWindow(800, 600, "Window name");
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
