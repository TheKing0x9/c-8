#include <raylib.h>
#include <stdio.h>

#include <chip8.h>

#define RAYGUI_IMPLEMENTATION
#include <lib/raygui.h>

static unsigned short available_speeds[0x5] = {
  8, 10, 12, 16, 20
};

int main(int argc, char** argv) {
  chip8 chip = { 0 };

  int selected_speed = 1;
  bool speed_dropdown_enabled = false;
  bool cycle = false;
  unsigned short speed = available_speeds[selected_speed];
  unsigned short opcode = 0;
  char status_text[100] = { 0 };

  chip8_init(&chip);
  if (argc > 1) {
    if (!chip8_loadrom(&chip, argv[1])) {
      printf("The ROM could not be loaded ... \n");
      return 1;
    } else {
      cycle = true;
    }
  }

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(chip.ren.width, chip.ren.height + 50, "Chip8-Emulator");
  SetTargetFPS(60);

  while(!WindowShouldClose()) {
    if (IsFileDropped())
    {
      int dropFileCount = 0;
      char **droppedFiles = GetDroppedFiles(&dropFileCount);

      if ((dropFileCount > 0) && IsFileExtension(droppedFiles[0], ".ch8")) {
        printf("Loading ROM %s\n", droppedFiles[0]);
        chip8_init(&chip);
        if (!chip8_loadrom(&chip, droppedFiles[0])){
          printf("The ROM could not be loaded ... \n");
          return 1;
        }
      }
      ClearDroppedFiles();    // Clear internal buffers
    }

    if (cycle) {
      for (int i = 0; i < speed; i++) {
        opcode = chip8_cycle(&chip);
        chip8_update_timers(&chip);
      }
    }

    BeginDrawing();
    ClearBackground(BLACK);
    chip8_render(&chip);
    // spped controls
    GuiLabel((Rectangle) { 0, 0, 50, 25}, "  Speed");
    if (GuiDropdownBox((Rectangle){ 50, 0, 125, 25 }, "8 Cycles/Frame;10 Cycles/Frame;12 Cycles/Frame;16 Cycles/Frame; 20 Cycles/Frame", &selected_speed, speed_dropdown_enabled)){
      speed = available_speeds[selected_speed];
      speed_dropdown_enabled = !speed_dropdown_enabled;
    }
    // player controls
    if (GuiButton((Rectangle){ GetScreenWidth() / 2 - 12.5, 0, 25, 25 }, GuiIconText(cycle ? RAYGUI_ICON_PLAYER_PAUSE : RAYGUI_ICON_PLAYER_PLAY, ""))) cycle = !cycle;
    if (!cycle) {
      if (GuiButton((Rectangle){ GetScreenWidth() / 2 + 25, 0, 25, 25 }, GuiIconText(RAYGUI_ICON_PLAYER_NEXT, ""))) {
        opcode = chip8_cycle(&chip);
        chip8_update_timers(&chip);
      }
    }
    sprintf(status_text, "Executing instruction %04x at 0x%04x; I = %04x; Stack Pointer = %d", opcode, chip.pc, chip.i, chip.stack_pointer);
    GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 25, GetScreenWidth(), 25 }, status_text);
    EndDrawing();

    fflush(stdout);
  }

  CloseWindow();

  return 0;
}
