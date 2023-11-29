#include <stdint.h>
#include <errno.h>

#include "drivers/i2c.h"
#include "drivers/pcf8561.h"

/**
 * References
 *
 * https://www.nxp.com/docs/en/data-sheet/PCF8591.pdf
 */

int pcf8561_read(int fd, pcf8561_data *data) {
  i2c_buf buf;
  // output disabled; four single-ended input; auto-increment on; A/D channel 0
  uint8_t tx_buf[] = { 0b00000100 };
  uint8_t rx_buf[PCF8561_CH_CNT];

  buf.tx_buf = tx_buf;
  buf.tx_len = sizeof(tx_buf);
  buf.rx_buf = rx_buf;
  buf.rx_len = sizeof(rx_buf);

  int ret = i2c_transfer(fd, PCF8561_I2C_ADDR, &buf);

  // calculate data
  data->ain0 = rx_buf[1];
  data->ain1 = rx_buf[2];
  data->ain2 = rx_buf[3];
  data->ain3 = rx_buf[0];

  return ret;
}
