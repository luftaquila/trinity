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

/* global variables */
payload_t payload;

/* function prototypes */
void *thread_job_sensor(void *arg);
void *thread_job_socket(void *arg);

/**************************************************************************
 * main function
 */
int main(void) {
  pthread_t thread_sensor;
  pthread_t thread_socket;

  payload.id = ID_WATERLEVEL;

  if (pthread_create(&thread_sensor, NULL, thread_job_sensor, NULL) < 0) {
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
 * Measures the sensor reading
 */
void *thread_job_sensor(void *arg) {
  printf("[SENSOR] setting up an I2C bus...\n");

  int i2c1;
  int ret = i2c_register(I2C1, &i2c1);

  if (ret < 0) {
    printf("[ERR] I2C register failed: %s\n", strerror(-ret));
  } else {
    printf("[INF] I2C register: %d\n", ret);
  }

  pcf8561_data data;

  printf("[SENSOR] collecting sensor data...\n");

  while (1) {
    ret = pcf8561_read(i2c1, &data);

    if (ret < 0) {
      printf("[ERR] PCF8561 read failed: %s\n", strerror(-ret));
    } else {
      printf("[INF] PCF8561 read: %d %d %d %d\n", data.ain0, data.ain1, data.ain2, data.ain3);

      payload.note = data.ain2;
      payload.volume = data.ain3;
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
    ret = write(sock, &payload, sizeof(payload));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SERVER] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload.id, payload.note, payload.volume);

    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *) -1);
}
