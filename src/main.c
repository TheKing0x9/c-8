#include <raylib.h>

#include <chip8.h>

int main(int argc, char** argv) {
  chip8 chip = { 0 };
  chip8_init(&chip);

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(chip.ren.width, chip.ren.height, "Chip8-Emulator");
  SetTargetFPS(60);

  while(!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    chip8_render(&chip);
    EndDrawing();

    fflush(stdout);
  }

  CloseWindow();

  return 0;
}
