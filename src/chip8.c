#include <renderer.h>
#include <keyboard.h>
#include <chip8.h>

#include <raylib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
  ren_clear(&chip->ren);
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

unsigned short chip8_cycle(chip8* chip, unsigned short speed) {
  unsigned short opcode = 0;
  for (int i = 0; i < speed; i++) {
    opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
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
      case 0x3000 :
        if (chip->v[x] == nn(opcode)) chip->pc += 2;
        break;
      case 0x4000 :
        if (chip->v[x] != nn(opcode)) chip->pc += 2;
        break;
      case 0x5000 :
        if (chip->v[x] == chip->v[y]) chip->pc += 2;
        break;
      case 0x6000 :
        chip->v[x] = nn(opcode);
        break;
      case 0x7000 :
        chip->v[x] += nn(opcode);
        break;
      case 0x8000 :
        switch(n(opcode)) {
          case 0x0: ;
            chip->v[x] = chip->v[y];
            break;
          case 0x1:
            chip->v[x] |= chip->v[y];
            break;
          case 0x2:
            chip->v[x] &= chip->v[y];
            break;
          case 0x3:
            chip->v[x] ^= chip->v[y];
            break;
          case 0x4: ;
            unsigned short sum = chip->v[x] + chip->v[y];
            chip->v[0xF] = (sum > 0xFF);
            chip->v[x] += chip->v[y];
            break;
          case 0x5:
            chip->v[0xF] = chip->v[x] > chip->v[y];
            chip->v[x] -= chip->v[y];
            break;
          case 0x6:
            chip->v[0xF] = chip->v[x] & 0x1;
            chip->v[x] >>= 1;
            break;
          case 0x7:
            chip->v[0xF] = chip->v[y] > chip->v[x];
            chip->v[x] = chip->v[y] - chip->v[x];
            break;
          case 0xE:
            chip->v[0xF] = chip->v[x] & 0x80;
            chip->v[x] <<= 1;
            break;
          default:
            printf("Unknown opcode %04x\n", opcode);
        }
        break;
      case 0x9000 :
        if (chip->v[x] != chip->v[y]) chip->pc += 2;
        break;
      case 0xA000 :
        chip->i = nnn(opcode);
        break;
      case 0xB000 :
        chip->pc = nnn(opcode) + chip->v[0];
        break;
      case 0xC000 : ;
        unsigned short random = rand() * 0xFF;
        chip->v[x] = random & nn(opcode);
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
      case 0xE000 :
        switch (nn(opcode)) {
          case 0x9E :
            if (is_key_pressed(chip->v[x])) chip->pc += 2;
            break;
          case 0xA1 :
            if (!is_key_pressed(chip->v[x])) chip->pc += 2;
            break;
        }
        break;
      case 0xF000 :
        switch(nn(opcode)) {
          case 0x07:
            chip->v[x] = chip->delay_timer;
            break;
          case 0x0A:
            if (!any_key_pressed()) chip->pc -= 2;
            break;
          case 0x15:
            chip->delay_timer = chip->v[x];
            break;
          case 0x18:
            chip->sound_timer = chip->v[x];
            break;
          case 0x1E:
            chip->i += chip->v[x];
            break;
          case 0x29:
            chip->i = chip->v[x] * 5;
            break;
          case 0x33:
            chip->memory[chip->i] = chip->v[x] / 100;
            chip->memory[chip->i + 1] = (chip->v[x] / 10) % 10;
            chip->memory[chip->i + 2] = chip->v[x] % 10;
            break;
          case 0x55:
            for (unsigned short i = 0; i <= x; i++)
              chip->memory[chip->i + i] = chip->v[i];
            break;
          case 0x65:
            for (unsigned short i = 0; i <= x; i++)
              chip->v[i] = chip->memory[chip->i + i];
            break;
        }
        break;
      default :
        printf("Unknown opcode %04x\n", opcode); getchar();
    }
  }
  return opcode;
}
