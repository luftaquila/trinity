#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include <stdint.h>

/* RPi has only 2 I2C buses (https://pinout.xyz/pinout/i2c) */
#define I2C1 "/dev/i2c-1"
#define I2C2 "/dev/i2c-2"

/**
 * @struct i2c_buf
 * I2C read/write buffer
 *
 * @var   i2c_buf::tx_buf
 *          A transmit buffer of I2C transfer
 * @var   i2c_buf::tx_len
 *          A length of the transmit buffer
 * @var   i2c_buf::rx_buf
 *          A receive buffer of I2C transfer
 * @var   i2c_buf::rx_len
 *          A length of the receive buffer
 */
typedef struct {
  uint8_t *tx_buf;
  int tx_len;

  uint8_t *rx_buf;
  int rx_len;
} i2c_buf;


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

/**
 * Perform a write-and-read transfer without STOP through a I2C bus
 *
 * @param[in]     fd    The file descriptor of the device to read
 * @param[in]     addr  The address of the target I2C slave
 * @param[inout]  buf   The r/w buffer for combined transfer
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int i2c_transfer(int fd, int addr, i2c_buf *buf);

#endif /* __DRIVER_I2C_H */
