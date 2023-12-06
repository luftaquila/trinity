#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

/* MOCKUP SERVER configuration */
/* #define SERVER_MOCK 1 */

#include "types.h"
#include "drivers/i2c.h"
#include "drivers/pcf8561.h"
#include "drivers/adxl345.h"
#include "drivers/ledmatrix.h"

/* global variables */
// i2c bus and lock
int i2c1;
pthread_mutex_t lock;

// socket payload
payload_t payload_adc;
payload_t payload_accel;

// LED display value
int display_data[4];

// target remote server
struct sockaddr_in server;

/* function prototypes */
void *thread_job_adc(void *arg);
void *thread_job_accel(void *arg);
void *thread_job_display(void *arg);

/**************************************************************************
 * main function
 */
int main(void) {
  // set remote server
  init_socket_server(&server, SERVER_IP, SERVER_PORT);
  printf("[  MAIN] server configuration: %s:%d\n", SERVER_IP, SERVER_PORT);

  // configure i2c bus
  int ret = i2c_register(I2C1, &i2c1);

  if (ret < 0) {
    printf("[   ERR] I2C register failed: %s\n", strerror(-ret));
  } else {
    printf("[   INF] I2C bus initialized: %d\n", ret);
  }

  // spawn threads
  pthread_t thread_adc;
  pthread_t thread_accel;
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

  // LED display thread
  if (pthread_create(&thread_display, NULL, thread_job_display, NULL) < 0) {
    goto thread_fail;
  }

  pthread_join(thread_adc, NULL);

  return 0;

thread_fail:
  printf("Thread creation failed. Terminating...\n");
  return -1;
}


/**************************************************************************
 * Measures the PCF8561 ADC reading
 */
void *thread_job_adc(void *arg) {
  printf("[   ADC] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  printf("[   ADC] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  printf("[   ADC] collecting PCF8561 ADC data...\n");
  pcf8561_data data;

  while (1) {
    pthread_mutex_lock(&lock);
    ret = pcf8561_read(i2c1, &data);
    pthread_mutex_unlock(&lock);

    if (ret < 0) {
      printf("[   ERR] PCF8561 read failed: %s\n", strerror(-ret));
    } else {
      payload_adc.note = data.ain2;
      payload_adc.volume = (payload_adc.note < 30 ) ? 0 : data.ain3;
    }

    // skip transmission on empty note
    if (payload_adc.note > 30) {
      ret = write(sock, &payload_adc, sizeof(payload_t));

      if (ret < 0) {
        printf("[   ADC] write failed: %s\n", strerror(errno));
      }

      printf("[   ADC] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_adc.id, payload_adc.note, payload_adc.volume);
    }

    usleep(100000); // 100ms delay
  }

socket_fail:
  printf("[   ADC] socket connection failed\n");
  pthread_exit((void *) -1);
}


/**************************************************************************
 * Measures the ADXL345 accelerometer reading
 */
void *thread_job_accel(void *arg) {
  printf("[ ACCEL] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  printf("[ ACCEL] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  printf("[ ACCEL] collecting ADXL345 accelerometer data...\n");
  adxl345_data data;

  pthread_mutex_lock(&lock);
  ret = adxl345_setup(i2c1);
  pthread_mutex_unlock(&lock);

  if (ret < 0) {
    printf("[   ERR] ADXL345 setup failed: %s\n", strerror(-ret));
  } else {
    printf("[ ACCEL] ADXL345 setup: %d\n", ret);
  }

  while (1) {
    pthread_mutex_lock(&lock);
    ret = adxl345_read(i2c1, &data);
    pthread_mutex_unlock(&lock);

    if (ret < 0) {
      printf("[   ERR] ADXL345 read failed: %s\n", strerror(-ret));
    } else {
      // filter positive G and fit to data range
      double vector = sqrt(pow(data.x, 2) + pow(data.y, 2) + pow(data.z, 2));
      vector = fabs(vector - 1.0) * 20.0; // postprocessing g vector
      payload_accel.note = (int)vector;
      payload_accel.volume = (payload_accel.note < 30) ? 0 : 80; // fixed volume
    }

    // skip transmission on empty note
    if (payload_accel.note > 30) {
      ret = write(sock, &payload_accel, sizeof(payload_t));

      if (ret < 0) {
        printf("[ ACCEL] write failed: %s\n", strerror(errno));
      }

      printf("[ ACCEL] write(%d): { id: %d, note: %d, volume: %d }\n", ret, payload_accel.id, payload_accel.note, payload_accel.volume);
    }

    usleep(100000); // 100ms delay
  }

socket_fail:
  printf("[   ADC] socket connection failed\n");
  pthread_exit((void *) -1);
}


/**************************************************************************
 * read data from server and draw on the 160*16 LED display
 */
void *thread_job_display(void *arg) {
  printf("[   LED] setting up a panel...\n");

  int ret = ledmatrix_setup();

  if (ret < 0) {
    printf("[   ERR] LED setup failed: %d\n", ret);
    return NULL;
  }

  printf("[   LED] server: %s\n", SERVER_IP);
  printf("[   LED] initiating socket...\n");

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT + 1);

  printf("[   LED] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

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
    /* usleep(20000); // 50Hz update rate */
  }

socket_fail:
  printf("[   LED] socket connection failed\n");
  pthread_exit((void *) -1);
}
