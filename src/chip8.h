#ifndef CHIP8_H
  #define CHIP8_H

  #include <renderer.h>

  #define MEMORY_SIZE 4096

  struct chip8_settings {
    unsigned short shift_ignore_vy;
    unsigned short load_changes_i;
    unsigned short jump_with_offset;
    unsigned short clear_vf;
    unsigned short clip_sprites;
  };
  typedef struct chip8_settings chip8_settings;

  struct chip8 {
    unsigned short pc;
    unsigned short i;
    unsigned short stack[0x10];

    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned char stack_pointer;
    unsigned char v[0x10];
    unsigned char memory[MEMORY_SIZE];

    renderer ren;
    chip8_settings settings;
  };
  typedef struct chip8 chip8;

  void chip8_init(chip8*, unsigned short);
  void chip8_render(chip8*);
  void chip8_update_timers(chip8*);

  int chip8_loadrom(chip8*, char*);
  unsigned short chip8_cycle(chip8*);
#endif
