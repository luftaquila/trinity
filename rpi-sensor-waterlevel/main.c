#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

/* MOCKUP SERVER configuration */
#define SERVER_MOCK 1

#include "types.h"
#include "drivers/i2c.h"
#include "drivers/pcf8561.h"
#include "drivers/adxl345.h"

/* global variables */
int i2c1;
payload_t payload_adc;
payload_t payload_accel;

/* function prototypes */
void *thread_job_adc(void *arg);
void *thread_job_accel(void *arg);
void *thread_job_socket(void *arg);

/**************************************************************************
 * main function
 */
int main(void) {
  printf("[  MAIN ] setting up a I2C bus...\n");

  int ret = i2c_register(I2C1, &i2c1);

  if (ret < 0) {
    printf("[ERR] I2C register failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C register: %d\n", ret);
  }

  pthread_t thread_sensor;
  pthread_t thread_socket;

  payload_adc.id = ID_WATERLEVEL;
  payload_accel.id = ID_ACCEL;

  if (pthread_create(&thread_sensor, NULL, thread_job_adc, NULL) < 0) {
    goto thread_fail;
  }

  if (pthread_create(&thread_sensor, NULL, thread_job_accel, NULL) < 0) {
    goto thread_fail;
  }

  if (pthread_create(&thread_socket, NULL, thread_job_socket, NULL) < 0) {
    goto thread_fail;
  }

  pthread_join(thread_socket, NULL);

  return 0;

thread_fail:
  printf("Thread creation failed. Terminating...\n");
  return -1;
}


/**************************************************************************
 * Measures the PCF8561 ADC reading
 */
void *thread_job_adc(void *arg) {
  pcf8561_data data;

  printf("[   ADC] collecting PCF8561 ADC data...\n");

  while (1) {
    // TODO: lock system required
    int ret = pcf8561_read(i2c1, &data);

    if (ret < 0) {
      printf("[ERR] PCF8561 read failed: %s\n", strerror(-ret));
    } else {
      payload_adc.note = data.ain2;
      payload_adc.volume = data.ain3;
    }

    usleep(100000);
  }
}


/**************************************************************************
 * Measures the ADXL345 accelerometer reading
 */
void *thread_job_accel(void *arg) {
  adxl345_data data;

  // TODO: lock system required
  int ret = adxl345_setup(i2c1);

  if (ret < 0) {
    printf("[ERR] ADXL345 setup failed: %s\n", strerror(-ret));
  } else {
    printf("[ ACCEL] ADXL345 setup: %d\n", ret);
  }

  printf("[ ACCEL] collecting ADXL345 accelerometer data...\n");

  while (1) {
    // TODO: lock system required
    ret = adxl345_read(i2c1, &data);

    if (ret < 0) {
      printf("[ERR] PCF8561 read failed: %s\n", strerror(-ret));
    } else {
      // TODO: fine tuning data range required
      payload_accel.note = data.x;
      payload_accel.volume = data.y;
    }

    usleep(100000);
  }
}


/**************************************************************************
 * Transmit sensor reading via socket
 */
void *thread_job_socket(void *arg) {
  printf("[SOCKET] server: %s\n", SERVER_IP);
  printf("[SOCKET] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT);

  printf("[SOCKET] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  if (ret < 0) {
    goto socket_fail;
  }

  while (1) {
    /* ADC value transmission */
    ret = write(sock, &payload_adc, sizeof(payload_t));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SERVER] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_adc.id, payload_adc.note, payload_adc.volume);

    /* acceleromter value transmission */
    ret = write(sock, &payload_accel, sizeof(payload_t));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SERVER] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_accel.id, payload_accel.note, payload_accel.volume);

    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *) -1);
}
