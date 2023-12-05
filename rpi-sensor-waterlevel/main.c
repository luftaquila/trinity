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
#include "drivers/ledmatrix.h"

/* global variables */
int i2c1;
pthread_mutex_t lock;
payload_t payload_adc;
payload_t payload_accel;
int display_data[4];

/* function prototypes */
void *thread_job_adc(void *arg);
void *thread_job_accel(void *arg);
void *thread_job_socket(void *arg);
void *thread_job_display(void *arg);

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

  pthread_t thread_adc;
  pthread_t thread_accel;
  pthread_t thread_socket;
  pthread_t thread_display;

  payload_adc.id = ID_WATERLEVEL;
  payload_accel.id = ID_ACCEL;

  // PCF8561 ADC measurement thread
  if (pthread_create(&thread_adc, NULL, thread_job_adc, NULL) < 0) {
    goto thread_fail;
  }

  // ADXL345 acceleromter measurement thread
  if (pthread_create(&thread_accel, NULL, thread_job_accel, NULL) < 0) {
    goto thread_fail;
  }

  // socket communication thread
  if (pthread_create(&thread_socket, NULL, thread_job_socket, NULL) < 0) {
    goto thread_fail;
  }

  // LED display thread
  if (pthread_create(&thread_display, NULL, thread_job_display, NULL) < 0) {
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
    pthread_mutex_lock(&lock);
    int ret = pcf8561_read(i2c1, &data);
    pthread_mutex_unlock(&lock);

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

  pthread_mutex_lock(&lock);
  int ret = adxl345_setup(i2c1);
  pthread_mutex_unlock(&lock);

  if (ret < 0) {
    printf("[ERR] ADXL345 setup failed: %s\n", strerror(-ret));
  } else {
    printf("[ ACCEL] ADXL345 setup: %d\n", ret);
  }

  printf("[ ACCEL] collecting ADXL345 accelerometer data...\n");

  while (1) {
    pthread_mutex_lock(&lock);
    ret = adxl345_read(i2c1, &data);
    pthread_mutex_unlock(&lock);

    if (ret < 0) {
      printf("[ERR] ADXL345 read failed: %s\n", strerror(-ret));
    } else {
      // filter positive G and fit to data range
      payload_accel.note = (int)((data.z < 0.0) ? (data.z * -1.0) : 0.0) * 20;
      payload_accel.volume = 80; // fixed volume
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

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT);

  printf("[SOCKET] waiting for server...\n");
  int ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  if (ret < 0) {
    goto socket_fail;
  }

  while (1) {
    /* ADC value transmission */
    ret = write(sock, &payload_adc, sizeof(payload_t));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_adc.id, payload_adc.note, payload_adc.volume);

    /* acceleromter value transmission */
    ret = write(sock, &payload_accel, sizeof(payload_t));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_accel.id, payload_accel.note, payload_accel.volume);

    /* read and draw display graph */
    ret = read(sock, display_data, sizeof(display_data));
    ledmatrix_drawgraph(display_data, 4);

    usleep(100000); // transmission delay 100ms
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *) -1);
}


/**************************************************************************
 * read data from server and draw on the 160*16 LED display
 */
void *thread_job_display(void *arg) {
  printf("[   LED] server: %s\n", SERVER_IP);
  printf("[   LED] initiating socket...\n");

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT + 1);

  printf("[   LED] waiting for server...\n");
  int ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  if (ret < 0) {
    goto socket_fail;
  }

  while (1) {
    /* read and draw display graph */
    ret = read(sock, display_data, sizeof(display_data));

    /* adjust graph range */
    for (int i = 0; i < 4; i++) {
      display_data[i] += 20;
    }

    ledmatrix_drawgraph(display_data, 4);

    usleep(20000); // 50Hz update
  }

socket_fail:
  printf("[   LED] socket connection failed\n");
  pthread_exit((void *) -1);
}
