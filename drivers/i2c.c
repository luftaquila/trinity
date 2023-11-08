#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "drivers/i2c.h"

/**
 * References
 *
 * open(2), close(2), read(2), write(2), ioctl(2), errno(3)
 * https://www.kernel.org/doc/Documentation/i2c/dev-interface
 */

int i2c_register(const char *bus, int *fd) {
  *fd = open(bus, O_RDWR);

  if (*fd < 0) {
    return -errno;
  }

  return 0;
}

int i2c_unregister(int fd) {
  int ret = close(fd);

  if (ret < 0) {
    return -errno;
  }

  return 0;
}

int i2c_write(int fd, int addr, uint8_t *buf, int len) {
  int ret = ioctl(fd, I2C_SLAVE, addr);

  if (ret < 0) {
    return -errno;
  }

  if (write(fd, buf, len) != len) {
    return -ETIMEDOUT;
  }

  return 0;
}

int i2c_read(int fd, int addr, uint8_t *buf, int len) {
  int ret = ioctl(fd, I2C_SLAVE, addr);

  if (ret < 0) {
    return -errno;
  }

  if (read(fd, buf, len) != len) {
    return -ETIMEDOUT;
  }

  return 0;
}

int i2c_transfer(int fd, int addr, i2c_buf *buf) {
  struct i2c_rdwr_ioctl_data payload;
  struct i2c_msg msg[2];

  payload.msgs = msg;
  payload.nmsgs = 2;

  /* write transfer */
  msg[0].addr = addr;
  msg[0].flags = 0;
  msg[0].len = buf->tx_len;
  msg[0].buf = buf->tx_buf;

  /* read transfer without STOP */
  msg[1].addr = addr;
  msg[1].flags = I2C_M_RD;
  msg[1].len = buf->rx_len;
  msg[1].buf = buf->rx_buf;

  int ret = ioctl(fd, I2C_RDWR, &payload);

  if (ret < 0) {
    return -errno;
  }

  return 0;
}
