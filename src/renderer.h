#ifndef RENDERER_H
  #define RENDERER_H

  #define ROWS 32
  #define COLS 64

  struct renderer {
    unsigned int scale;
    unsigned int rows;
    unsigned int columns;

    unsigned int width;
    unsigned int height;

    unsigned char display[ROWS * COLS];
  };
  typedef struct renderer renderer;

  void ren_init(renderer*, unsigned int);
  void ren_clear(renderer*);
  void ren_render(renderer*);

  int ren_set_pixel(renderer* ren, unsigned int x, unsigned int y);
#endif

