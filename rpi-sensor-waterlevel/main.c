#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>

#include "types.h"
#include "drivers/i2c.h"
#include "drivers/adxl345.h"

int main(void) {
  int i2c1;
  int ret = i2c_register(I2C1, &i2c1);

  if (ret < 0) {
    printf("[ERR] I2C register failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C register: %d\n", ret);
  }

  adxl345_data data;

  for (int i = 0; i < 10; i++) {
    ret = adxl345_read(i2c1, &data);

    if (ret < 0) {
      printf("[ERR] ADXL345 read failed: %s\n", strerror(-ret));
    } else {
      printf("[INF] ADXL345 read: %d\nx: %lf y: %lf z: %lf\n\n", ret, data.x, data.y, data.z);
    }
  }

  ret = i2c_unregister(i2c1);

  if (ret < 0) {
    printf("[ERR] I2C unregister failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C unregister: %d\n", ret);
  }

  return 0;
}
