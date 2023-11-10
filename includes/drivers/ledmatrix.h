#ifndef __DRIVER_LEDMATRIX_H
#define __DRIVER_LEDMATRIX_H

#define LED_MATRIX_ROWS   16
#define LED_MATRIX_COLS   32
#define LED_MATRIX_CHAINS 5

/**
 * Setup 32*16*5 RGB LED matrix
 *
 * @return    success: 0
 *            fail: nonzero
 */
int ledmatrix_setup(void);

/**
 * Draw 4 graphs on the matrix
 *
 * @param[in]     arr   The array containing graph values
 * @param[in]     len   The array length. Must be 4
 *
 * @return    success: 0
 *            fail: nonzero
 */
int ledmatrix_drawgraph(int *arr, int len);

#endif /* __DRIVER_LEDMATRIX_H */
