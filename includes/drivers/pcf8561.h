#ifndef __DRIVER_PCF8561_H
#define __DRIVER_PCF8561_H

#include <stdint.h>

#define PCF8561_I2C_ADDR 0x48
#define PCF8561_CH_CNT   4

/**
 * @struct pcf8561_data
 * PCF8561 analog data
 */
typedef struct {
  uint8_t ain0;
  uint8_t ain1;
  uint8_t ain2;
  uint8_t ain3;
} pcf8561_data;


/**
 * Read ADC conversion data from the PCF8561
 *
 * @param[in]     fd    The file descriptor of the I2C bus with PCF8561
 * @param[out]    data  The ADC readings
 *
 * @return    success: 0
 *            fail: `-errno`
 */
int pcf8561_read(int fd, pcf8561_data *data);

#endif /* __DRIVER_PCF8561_H */
