#include <renderer.h>

#include <raylib.h>
#include <string.h>

void ren_init(renderer* ren, unsigned int scale) {
  ren->scale = scale;
  ren->columns = COLS;
  ren->rows = ROWS;
  ren->width = COLS * scale;
  ren->height = ROWS * scale;
}

void ren_clear(renderer* ren) {
  memset(ren->display, 0, sizeof(ren->display));
}

int ren_set_pixel(renderer* ren, unsigned int x, unsigned int y) {
  if (x >= ren->columns) x -= ren->columns;
  else if (x < 0) x += ren->columns;

  if (y >= ren->rows) y -= ren->rows;
  else if (y < 0) y += ren->rows;

  unsigned int pixel = x + y * ren->columns;
  ren->display[pixel] ^= 1;
  return !ren->display[pixel];
}

void ren_render(renderer* ren) {
  for (int i = 0; i < ren->columns; i++)
    for (int j = 0; j < ren->rows; j++)
      if (ren->display[i + j * ren->columns])
        DrawRectangle(i * ren->scale, j * ren->scale + 25, ren->scale, ren->scale,
                      (Color){ 198, 43, 105, 255 });
}
