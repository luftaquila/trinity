#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include "types.h"
#include "drivers/i2c.h"
#include "drivers/adxl345.h"

#include "led-matrix-c.h"

int main(void) {
  /* configure 32*16*5 RGB LED Matrix board */
  struct RGBLedMatrixOptions options;
  memset(&options, 0, sizeof(options));
  options.rows = 16;
  options.chain_length = 5;

  /* init matrix */
  struct RGBLedMatrix *matrix;
  matrix = led_matrix_create_from_options(&options, NULL, NULL);
  if (matrix == NULL) {
    return -1;
  }

  /* buffer canvas */
  struct LedCanvas *offscreen_canvas;
  int width, height;
  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);
  led_canvas_get_size(offscreen_canvas, &width, &height);

  printf("Size: %dx%d. Hardware gpio mapping: %s\n",
        width, height, options.hardware_mapping);

  return 0;
}
