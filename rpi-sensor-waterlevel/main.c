#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>

#include "types.h"
#include "drivers/i2c.h"

int main(void) {
  int i2c1;
  int ret = i2c_register(I2C1, &i2c1);

  if (ret < 0) {
    printf("[ERR] I2C register failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C register: %d\n", ret);
  }


  uint8_t tx_buffer[3] = { 0x01, 0x02, 0x03 };
  ret = i2c_write(i2c1, 0x48, tx_buffer, sizeof(tx_buffer));

  if (ret < 0) {
    printf("[ERR] I2C write failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C write: %d\n", ret);
  }


  uint8_t rx_buffer[3];
  ret = i2c_read(i2c1, 0x48, rx_buffer, sizeof(rx_buffer));

  if (ret < 0) {
    printf("[ERR] I2C read failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C read: %d\n", ret);
  }


  ret = i2c_unregister(i2c1);

  if (ret < 0) {
    printf("[ERR] I2C unregister failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C unregister: %d\n", ret);
  }

  return 0;
}
