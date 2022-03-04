#include <renderer.h>
#include <chip8.h>

#include <raylib.h>
#include <string.h>
#include <stdio.h>

#define n(opcode) (opcode & 0xF)
#define nn(opcode) (opcode & 0xFF)
#define nnn(opcode) (opcode & 0xFFF)

void chip8_init(chip8* chip) {
  chip->i = 0;
  chip->pc = 0x200;

  chip->sound_timer = 0;
  chip->delay_timer = 0;

  const unsigned char chip8_fontset[0x50] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memmove(chip->memory, chip8_fontset, sizeof(unsigned char) * 0x50);
  memset(chip->stack, 0, sizeof(chip->stack));
  memset(chip->v, 0, sizeof(chip->v));

  ren_init(&chip->ren, 10);
}

int chip8_loadrom(chip8* chip, char* path) {
  if (!FileExists(path)) { return 0; }
  FILE* game = fopen(path, "rb");
  if (!game) { return 0; }

  int read = fread(chip->memory + 0x200, 1, MEMORY_SIZE - 0x200, game);
  (void)read;

  return 1;
}

void chip8_update_timers(chip8* chip) {
  if (chip->sound_timer > 0) {
    printf("%c", 7);
    --chip->sound_timer;
  }

  if (chip->delay_timer > 0)
    --chip->delay_timer;
}

void chip8_render(chip8* chip) {
  ren_render(&chip->ren);
}

void chip8_cycle(chip8* chip, unsigned short speed) {
  for (int i = 0; i < speed; i++) {
    unsigned short opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
    unsigned char x = (opcode & 0x0F00) >> 8;
    unsigned char y = (opcode & 0x00F0) >> 4;

    chip->pc += 2;

    switch (opcode & 0xF000) {
      case 0x0000 :
        switch(nn(opcode)) {
          case 0xE0 :
            ren_clear(&chip->ren);
            break;
          case 0xEE :
            chip->pc = chip->stack[chip->stack_pointer-- & 0xF];
            break;
        }
        break;
      case 0x1000 :
        chip->pc = nnn(opcode);
        break;
      case 0x2000 :
        chip->stack[++chip->stack_pointer & 0xF] = chip->pc;
        chip->pc = nnn(opcode);
        break;
      case 0x6000 :
        chip->v[x] = nn(opcode);
        break;
      case 0x7000 :
        chip->v[x] += nn(opcode);
        break;
      case 0xA000 :
        chip->i = nnn(opcode);
        break;
      case 0xD000 : ;
        unsigned short width = 8;

        unsigned short dx = chip->v[x] & 63;
        unsigned short dy = chip->v[y] & 31;

        chip->v[0xF] = 0;
        for (unsigned short row = 0; row < n(opcode); row++) {
          unsigned char sprite = chip->memory[chip->i + row];
          for (unsigned short col = 0; col < width; col++) {
            if (sprite & (0x80)) {
              chip->v[0xF] =
                ren_set_pixel(&chip->ren, dx + col, dy + row);
            }
            sprite <<= 1;
          }
        }
        break;

    }
  }
}
