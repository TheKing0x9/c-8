#include <raylib.h>
#include <stdio.h>

#include <chip8.h>

#define RAYGUI_IMPLEMENTATION
#include <lib/raygui.h>

static unsigned short available_speeds[0x5] = {
  8, 10, 12, 16, 20
};

#define SCALE 10
#define ROWS 32

/* Unneccessary qol defines */
#define REN_WIDTH chip.ren.width
#define REN_HEIGHT chip.ren.height

static char** set_cpu_data(chip8* chip) {
  static char buffer[4096][10] = { 0 };
  static char *result[4096] = { 0 };

  for (unsigned short i = 0; i < 4096; i++) {
    sprintf(buffer[i], "%04x : %02x", i, chip->memory[i]);
    result[i] = buffer[i];
  }
  return result;
}

typedef struct {
  int scroll_index;
  int active;
} list_view;

int main(int argc, char** argv) {
  chip8 chip = { 0 };

  int selected_speed = 1;

  bool speed_dropdown_enabled = false;
  bool cycle = false;
  bool follow_cpu = false;
  bool quit = false;
  bool show_quit_window = false;

  unsigned short pc = 0x200;
  unsigned short index = 0;
  unsigned short speed = available_speeds[selected_speed];
  unsigned short opcode = 0;

  char status_text[100] = { 0 };
  char register_data[145] = { 0 };
  char** cpu_data = NULL;

  list_view registers = { 0xE, -1 };
  list_view stack = { 0, 2 };
  list_view cpu = { 0x0, 0x0 };

  chip8_init(&chip, SCALE);
  if (argc > 1) {
    if (!chip8_loadrom(&chip, argv[1])) {
      printf("The ROM could not be loaded ... \n");
      return 1;
    } else {
      cycle = true;
      cpu_data = set_cpu_data(&chip);
    }
  }

  /* Initialize Raylib */
  SetTraceLogLevel(LOG_ERROR);
  InitWindow(REN_WIDTH*2, REN_HEIGHT*2, "Chip8-Emulator");
  SetTargetFPS(60);
  SetExitKey(0);

  /* Store commonly used values */
  int half_width = REN_WIDTH / 2;
  int screen_height = GetScreenHeight();
  int screen_width = GetScreenWidth();

  /* Set renderer offset */
  chip.ren.offsetX = GetScreenWidth() * 0.5 - REN_WIDTH * 0.5;
  chip.ren.offsetY = 32;

  while(!quit) {
    quit = WindowShouldClose();

    if (IsKeyPressed(KEY_ESCAPE)) show_quit_window = !show_quit_window;
    /* Handle file drop */
    if (IsFileDropped())
    {
      int dropFileCount = 0;
      char **droppedFiles = GetDroppedFiles(&dropFileCount);

      if ((dropFileCount > 0)) {
        if (IsFileExtension(droppedFiles[0], ".ch8")) {
          printf("Loading ROM %s\n", droppedFiles[0]);
          chip8_init(&chip, SCALE);
          if (!chip8_loadrom(&chip, droppedFiles[0])){
            printf("The ROM could not be loaded ... \n");
            return 1;
          } else {
            cycle = true;
            cpu_data = set_cpu_data(&chip);
          }
        }
        else if (IsFileExtension(droppedFiles[0], ".rgs")) {
          GuiLoadStyle(droppedFiles[0]);
        }
      }
      ClearDroppedFiles();
    }

    if (cycle) {
      for (unsigned short i = 0; i < speed; i++) {
        pc = chip.pc; index = chip.i;
        opcode = chip8_cycle(&chip);
        chip8_update_timers(&chip);
      }
    }

    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    chip8_render(&chip);
    /* player controls */
    if (GuiButton((Rectangle){ screen_width / 2 - 12.5, 0, 25, 25 }, GuiIconText(cycle ? RAYGUI_ICON_PLAYER_PAUSE : RAYGUI_ICON_PLAYER_PLAY, ""))) cycle = !cycle;
    if (!cycle) {
      if (GuiButton((Rectangle){ screen_width / 2 + 25, 0, 25, 25 }, GuiIconText(RAYGUI_ICON_PLAYER_NEXT, ""))) {
        pc = chip.pc; index = chip.i;
        opcode = chip8_cycle(&chip);
        chip8_update_timers(&chip);
      }
    }
    /* register visualizer */
    unsigned short offset = 0;
    for (unsigned short i = 0; i < 0xE; i++) {
      sprintf(register_data + offset, "%x : %04x;", i, chip.v[i]);
      offset += 9;
    }
    sprintf(register_data + offset, "%x : %04x", 0xE, chip.v[0xE]);
    GuiGroupBox((Rectangle){ chip.ren.offsetX, chip.ren.offsetY + REN_HEIGHT + 16, half_width, REN_HEIGHT }, "Registers");
    registers.active = GuiListView((Rectangle){ chip.ren.offsetX + 2, chip.ren.offsetY + REN_HEIGHT + 20, half_width - 2, 224+32 }, register_data, &registers.scroll_index, registers.active);
    /* stack visualizer */
    stack.active = chip.stack_pointer;
    offset = 0;
    for (unsigned short i = 0; i < 0xE; i++) {
      sprintf(register_data + offset, "%x : %04x;", i, chip.stack[i]);
      offset += 9;
    }
    sprintf(register_data + offset, "%x : %04x", 0xE, chip.stack[0xE]);
    GuiGroupBox((Rectangle){ chip.ren.offsetX + half_width, chip.ren.offsetY + REN_HEIGHT + 16, half_width, REN_HEIGHT }, "Stack");
    stack.active = GuiListView((Rectangle){ chip.ren.offsetX  + half_width + 2, chip.ren.offsetY + REN_HEIGHT + 20, half_width - 2, 224+32 }, register_data, &stack.scroll_index, stack.active);
    /* CPU visualizer */
    follow_cpu = GuiCheckBox((Rectangle){ 10, chip.ren.offsetY + 10, 15, 15 }, "Follow CPU", follow_cpu);
    cpu.active = pc;
    if (follow_cpu) cpu.scroll_index = pc;
    GuiGroupBox((Rectangle){ 0, chip.ren.offsetY, half_width, REN_HEIGHT * 2 - chip.ren.offsetY * 2}, "C.P.U.");
    cpu.active = GuiListViewEx((Rectangle){ 2, chip.ren.offsetY + 32, half_width - 2, REN_HEIGHT * 2 - 108 }, cpu_data, 4096, NULL, &cpu.scroll_index, cpu.active);
    /* Settings */
    GuiGroupBox((Rectangle){ REN_WIDTH * 1.5, chip.ren.offsetY, half_width, REN_HEIGHT * 2 - chip.ren.offsetY * 2}, "Settings");
    chip.settings.shift_ignore_vy = GuiCheckBox((Rectangle){ REN_WIDTH * 1.5 + 10, chip.ren.offsetY + 10, 15, 15 }, "Shift modifies vX in place and ignores vY.", chip.settings.shift_ignore_vy);
    chip.settings.load_changes_i = GuiCheckBox((Rectangle){ REN_WIDTH * 1.5 + 10, chip.ren.offsetY + 35, 15, 15 }, "load and store instructions modify i.", chip.settings.load_changes_i);
    chip.settings.jump_with_offset = GuiCheckBox((Rectangle){ REN_WIDTH * 1.5 + 10, chip.ren.offsetY + 60, 15, 15 }, "4 high bits of target address determines \nthe offset register of jump0 instead of v0.", chip.settings.jump_with_offset);
    chip.settings.clear_vf = GuiCheckBox((Rectangle){ REN_WIDTH * 1.5 + 10, chip.ren.offsetY + 110, 15, 15 }, "Clear vF after &=, |= and ^= operations.", chip.settings.clear_vf);
    chip.settings.clip_sprites = GuiCheckBox((Rectangle){ REN_WIDTH * 1.5 + 10, chip.ren.offsetY + 135, 15, 15 }, "Clip sprites instead of warping them.", chip.settings.clip_sprites);
    /* speed controls */
    GuiLabel((Rectangle) { 0, 0, 50, 25}, "  Speed");
    if (GuiDropdownBox((Rectangle){ 50, 0, 125, 25 }, "8 Cycles/Frame;10 Cycles/Frame;12 Cycles/Frame;16 Cycles/Frame; 20 Cycles/Frame", &selected_speed, speed_dropdown_enabled)){
      speed = available_speeds[selected_speed];
      speed_dropdown_enabled = !speed_dropdown_enabled;
    }
    /* status bar */
    sprintf(status_text, "Executing instruction %04x at 0x%04x; I = %04x; Stack Pointer = %x", opcode, pc, index, chip.stack_pointer);
    GuiStatusBar((Rectangle){ 0, screen_height - 32, screen_width, 32 }, status_text);

    if (show_quit_window) {
      DrawRectangle(0, 0, screen_width, screen_height, Fade(RAYWHITE, 0.8f));
      int result = GuiMessageBox((Rectangle){ screen_width / 2 - 125, screen_height / 2 - 50, 250, 100 }, GuiIconText(RAYGUI_ICON_EXIT, "Close Window"), "Do you really want to exit?", "Yes;No");

      if ((result == 0) || (result == 2)) show_quit_window = false;
      else if (result == 1) quit = true;
    }
    EndDrawing();

    fflush(stdout);
  }

  CloseWindow();

  return 0;
}
