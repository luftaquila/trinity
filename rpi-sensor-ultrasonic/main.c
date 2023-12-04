#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "drivers/gpio.h"

/* MOCKUP SERVER configuration */
#define SERVER_MOCK 1
#include "types.h"

#define POUT 23
#define PIN 24
#define REPEAT_MAX 4

/* global variables */
int result = 0;
double filterValue = 0;
int repeat = 0;

/* function prototypes */
void *thread_job_sensor(void *arg);
void *thread_job_socket(void *arg);

int main(int argc, char *argv[])
{
  pthread_t thread_sensor;
  pthread_t thread_socket;

  printf("complete\n");

  if (pthread_create(&thread_sensor, NULL, thread_job_sensor, NULL) < 0)
  {
    printf("thread_job_sensor failed\n");
  }
  if (pthread_create(&thread_socket, NULL, thread_job_socket, NULL) < 0)
  {
    printf("thread_socket failed\n");
  }

  pthread_join(thread_sensor, NULL);

  return (0);
}

void *thread_job_sensor(void *arg)
{
  clock_t start_t, end_t;
  double time;

  printf("[SENSOR] collecting sensor data...\n");

  // Enable GPIO pins
  if (-1 == GPIOExport(POUT) || -1 == GPIOExport(PIN))
  {
    printf("gpio export err\n");
    goto job_fail;
  }
  // wait for writing to export file
  usleep(100000);

  // Set GPIO directions
  if (-1 == GPIODirection(POUT, OUT) || -1 == GPIODirection(PIN, IN))
  {
    printf("gpio direction err\n");
    goto job_fail;
  }

  // init ultrawave trigger
  GPIOWrite(POUT, 0);
  usleep(20000);

  do
  {
    if (-1 == GPIOWrite(POUT, 1))
    {
      printf("gpio write/trigger err\n");
      goto job_fail;
    }

    usleep(10);
    GPIOWrite(POUT, 0);

    while (GPIORead(PIN) == 0)
    {
      start_t = clock();
    }
    while (GPIORead(PIN) == 1)
    {
      end_t = clock();
    }

    time = (double)(end_t - start_t) / CLOCKS_PER_SEC; // ms
    double distance = time / 2 * 34000;

    printf("distance: %.2f\n", distance);

    filterValue += distance;
    printf("filterValue:  %.2f\n", filterValue);

    repeat++;

    if (repeat == REPEAT_MAX)
    {
      result = filterValue / 5;
      filterValue = 0;
      repeat = 0;
    }
    usleep(20000);
  } while (1);

  // Disable GPIO pins
  if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))
    goto job_fail;

  return NULL;

job_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *)-1);
}

void *thread_job_socket(void *arg)
{
  printf("[SOCKET] server: %s\n", SERVER_IP);
  printf("[SOCKET] initiating socket...\n");

  int ret;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    goto socket_fail;
  }

  struct sockaddr_in server;
  init_socket_server(&server, SERVER_IP, SERVER_PORT_3);

  printf("[SOCKET] waiting for server...\n");
  ret = connect(sock, (struct sockaddr *)&server, sizeof(server));

  if (ret < 0)
  {
    goto socket_fail;
  }

  while (1)
  {
    ret = write(sock, &result, sizeof(result));

    if (ret < 0)
    {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write: %d(%d)\n", result, ret);
    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *)-1);
}
