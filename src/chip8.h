#ifndef CHIP8_H
  #define CHIP8_H

  #include <renderer.h>

  struct chip8 {
    struct renderer ren;
  };
  typedef struct chip8 chip8;

  void chip8_init(chip8*);
  void chip8_render(chip8*);
#endif
