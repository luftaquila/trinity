#ifndef __DRIVER_ADXL345_H
#define __DRIVER_ADXL345_H

#define ADXL345_I2C_ADDR 0x53
#define ADXL345_REG_ADDR 0x32
#define ADXL345_REG_CNT  6

/**
 * @struct adxl345_data
 * ADXL345 x, y, z axis data
 *
 * @var   i2c_buf::x
 * @var   i2c_buf::y
 * @var   i2c_buf::z
 */
typedef struct {
  double x;
  double y;
  double z;
} adxl345_data;


/**
 * Setup ADXL345 accelerometer unit
 *
 * @param[in]     fd    The file descriptor of the I2C bus with ADXL345
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int adxl345_setup(int fd);

/**
 * Read an accelerometer data from the ADXL345
 *
 * @param[in]     fd    The file descriptor of the I2C bus with ADXL345
 * @param[out]    data  The accelerometer readings
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int adxl345_read(int fd, adxl345_data *data);

#endif /* __DRIVER_ADXL345_H */
