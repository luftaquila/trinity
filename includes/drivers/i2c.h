#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include <stdint.h>

/* RPi has only 2 I2C buses (https://pinout.xyz/pinout/i2c) */
#define I2C0 "/dev/i2c-0"
#define I2C1 "/dev/i2c-1"

/**
 * Register a I2C bus
 *
 * @param[in]   bus   The i2c device
 * @param[out]  fd    The file descriptor of the opened device
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int i2c_register(const char *bus, int *fd);

/**
 * Unregister a I2C bus
 *
 * @param[in]  fd    The file descriptor of the device to close
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int i2c_unregister(int fd);

/**
 * Write to a I2C bus
 *
 * @param[in]  fd    The file descriptor of the device to write
 * @param[in]  addr  The address of the target I2C slave
 * @param[in]  buf   The write buffer
 * @param[in]  len   The length of the data to write
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int i2c_write(int fd, int addr, uint8_t *buf, int len);

/**
 * Read from a I2C bus
 *
 * @param[in]  fd    The file descriptor of the device to read
 * @param[in]  addr  The address of the target I2C slave
 * @param[out] buf   The read buffer
 * @param[in]  len   The length of the data to read
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int i2c_read(int fd, int addr, uint8_t *buf, int len);

#endif /* __DRIVER_I2C_H */
