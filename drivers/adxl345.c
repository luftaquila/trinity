#include <stdint.h>
#include <errno.h>

#include "drivers/i2c.h"
#include "drivers/adxl345.h"

/**
 * References
 *
 * https://www.analog.com/media/en/technical-documentation/data-sheets/adxl345.pdf
 */

int adxl345_setup(int fd) {
  uint8_t setup[][2] = {
    { 0x31, 0x01 }, // DATA_FORMAT range +-4g
    { 0x2D, 0x00 }, // POWER_CTL bit reset
    { 0x2D, 0x08 }  // POWER_CTL set measure mode. 100hz default rate
  };

  int ret = 0;
  ret |= i2c_write(fd, ADXL345_I2C_ADDR, setup[0], 2);
  ret |= i2c_write(fd, ADXL345_I2C_ADDR, setup[1], 2);
  ret |= i2c_write(fd, ADXL345_I2C_ADDR, setup[2], 2);

  if (ret != 0) {
    return -errno;
  } else {
    return 0;
  }
}

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
  data->x = 4.0 / 512.0 * (double)(*(int16_t *)(rx_buf + 0));
  data->y = 4.0 / 512.0 * (double)(*(int16_t *)(rx_buf + 2));
  data->z = 4.0 / 512.0 * (double)(*(int16_t *)(rx_buf + 4));

  return ret;
}
