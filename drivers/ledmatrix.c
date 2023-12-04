#include <string.h>

#include "led-matrix-c.h"
#include "drivers/ledmatrix.h"

struct RGBLedMatrix *matrix;
struct LedCanvas *offscreen_canvas;
struct Color colors[4] = {
  { .r = 255, .g = 0,   .b = 0   },
  { .r = 85,  .g = 255, .b = 0   },
  { .r = 0 ,  .g = 170, .b = 255 },
  { .r = 255, .g =   0, .b = 255 },
};

int ledmatrix_setup(void) {
  /* configure RGB LED Matrix board */
  struct RGBLedMatrixOptions options;
  memset(&options, 0, sizeof(options));
  options.rows = LED_MATRIX_ROWS;
  options.cols = LED_MATRIX_COLS;
  options.chain_length = LED_MATRIX_CHAINS;

  /* init matrix */
  matrix = led_matrix_create_from_options(&options, NULL, NULL);
  if (matrix == NULL) {
    return -1;
  }

  /* buffer canvas */
  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

  return 0;
}

int ledmatrix_drawgraph(int *arr, int len) {
  /* graph value count(len) must be 4! */
  if (len != 4) {
    return -1;
  }

  for (int i = 0; i < len; i++) {
    for (int x = 0; x < arr[i]; x++) {
      for (int y = 0; y < LED_MATRIX_ROWS / len; y++) {
        led_canvas_set_pixel(offscreen_canvas, x, y + i * 4, colors[i].r, colors[i].g, colors[i].b);
      }
    }

    for (int x = arr[i]; x < LED_MATRIX_COLS * LED_MATRIX_CHAINS; x++) {
      for (int y = 0; y < LED_MATRIX_ROWS / len; y++) {
        led_canvas_set_pixel(offscreen_canvas, x, y + i * 4, 0, 0, 0);
      }
    }
  }

  /* update screen */
  offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
  return 0;
}
