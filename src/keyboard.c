#include <keyboard.h>
#include <raylib.h>

static unsigned int keymap[0x10] = {
  88, 49, 50, 51,
  81, 87, 69, 65,
  83, 68, 90, 67,
  52, 82, 70, 86,
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
