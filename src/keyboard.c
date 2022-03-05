#include <keyboard.h>
#include <raylib.h>

static unsigned int keymap[0x10] = {
  49, 50, 51, 52,
  81, 87, 69, 82,
  65, 83, 68, 70,
  90, 88, 67, 86,
};

unsigned char is_key_pressed(unsigned char keycode) {
  return IsKeyDown(keymap[keycode]);
}

unsigned char any_key_pressed(void) {
  for (int i = 0; i < 0x10; i++) {
    if (IsKeyPressed(keymap[i])) return 1;
  }
  return 0;
}
