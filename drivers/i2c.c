#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "drivers/i2c.h"

/**
 * References
 *
 * open(2), close(2), read(2), write(2), ioctl(2), errno(3)
 * https://elinux.org/Interfacing_with_I2C_Devices
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
  ioctl(fd, I2C_SLAVE, addr << 1);

  if (write(fd, buf, len) != len) {
    return -ETIMEDOUT;
  }

  return 0;
}

int i2c_read(int fd, int addr, uint8_t *buf, int len) {
  ioctl(fd, I2C_SLAVE, (addr << 1) | 0x1);

  if (read(fd, buf, len) != len) {
    return -ETIMEDOUT;
  }

  return 0;
}
