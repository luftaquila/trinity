#include <stdint.h>
#include <errno.h>

#include "drivers/i2c.h"
#include "drivers/adxl345.h"

/**
 * References
 *
 * https://www.analog.com/media/en/technical-documentation/data-sheets/adxl345.pdf
 */

int adxl345_read(int fd, adxl345_data *data) {
  i2c_buf buf;
  uint8_t tx_buf[] = { ADXL345_REG_ADDR };
  uint8_t rx_buf[ADXL345_REG_CNT];

  buf.tx_buf = tx_buf;
  buf.tx_len = sizeof(tx_buf);
  buf.rx_buf = rx_buf;
  buf.rx_len = sizeof(rx_buf);

  int ret = i2c_transfer(fd, ADXL345_I2C_ADDR, &buf);

  // calculate data
  data->x = 2 / 512 * *(int16_t *)(rx_buf + 0);
  data->y = 2 / 512 * *(int16_t *)(rx_buf + 2);
  data->z = 2 / 512 * *(int16_t *)(rx_buf + 4);

  return ret;
}
