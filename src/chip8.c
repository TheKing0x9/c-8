#include <renderer.h>
#include <chip8.h>

#include <raylib.h>

void chip8_init(chip8* chip) {
  ren_init(&chip->ren, 10);
}

void chip8_render(chip8* chip) {
  ren_render(&chip->ren);
}
