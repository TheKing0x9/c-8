#include <raylib.h>
#include <stdio.h>

#include <chip8.h>

int main(int argc, char** argv) {

  chip8 chip = { 0 };
  chip8_init(&chip);
  if (!chip8_loadrom(&chip, argv[1])){
    printf("The ROM could not be loaded ... \n");
    return 1;
  }

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(chip.ren.width, chip.ren.height, "Chip8-Emulator");
  SetTargetFPS(60);

  unsigned short speed = 10;

  while(!WindowShouldClose()) {
    chip8_cycle(&chip, speed);
    chip8_update_timers(&chip);

    BeginDrawing();
    ClearBackground(BLACK);
    chip8_render(&chip);
    EndDrawing();

    fflush(stdout);
  }

  CloseWindow();

  return 0;
}
