#include <stdio.h>
#include <stdlib.h>
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

// #define SERVER_MOCK 1
#include "types.h"

#define POUT 23
#define PIN 24
#define POUT2 5
#define PIN2 6
#define REPEAT_MAX 5

pthread_mutex_t decay_number_lock = PTHREAD_MUTEX_INITIALIZER;

payload_t result;
double filterValue = 0;

void *thread_job_sensor(void *arg);
void *thread_job_sensor2(void *arg);
void *thread_job_socket(void *arg);

int main(int argc, char *argv[])
{
  pthread_t thread_sensor;
  pthread_t thread_socket;

  printf("complete\n");
  result.id = ID_ULTRASONIC;

  if (pthread_create(&thread_sensor, NULL, thread_job_sensor, NULL) < 0)
  {
    printf("thread_job_sensor failed\n");
  }
  if (pthread_create(&thread_sensor, NULL, thread_job_sensor2, NULL) < 0)
  {
    printf("second thread_job_sensor failed\n");
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
  int repeat1 = 0;

  printf("[SENSOR] collecting sensor data1...\n");

  if (-1 == GPIOExport(POUT) || -1 == GPIOExport(PIN))
  {
    printf("gpio export err\n");
    goto job_fail;
  }

  usleep(20000);

  if (-1 == GPIODirection(POUT, OUT) || -1 == GPIODirection(PIN, IN))
  {
    printf("gpio direction err\n");
    goto job_fail;
  }

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
    double distance = (time / 2) * 34000;

    filterValue += distance;

    repeat1++;

    if (repeat1 == REPEAT_MAX)
    {
      int val = ((int)filterValue / 5);
      if (val <= 150)
      {
        result.note = val;
      }
      printf("result note: %d\n", result.note);
      filterValue = 0;
      repeat1 = 0;
    }
    usleep(20000);
  } while (1);

  if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))
    goto job_fail;

  return NULL;

job_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *)-1);
}

void *thread_job_sensor2(void *arg)
{
  {
    clock_t start_t, end_t;
    double time;
    int repeat2 = 0;

    printf("[SENSOR] collecting sensor data2...\n");

    if (-1 == GPIOExport(POUT2) || -1 == GPIOExport(PIN2))
    {
      printf("gpio export err\n");
      goto job_fail;
    }
    usleep(100000);

    if (-1 == GPIODirection(POUT2, OUT) || -1 == GPIODirection(PIN2, IN))
    {
      printf("gpio direction err\n");
      goto job_fail;
    }

    GPIOWrite(POUT2, 0);
    usleep(100000);

    do
    {
      if (-1 == GPIOWrite(POUT2, 1))
      {
        printf("gpio write/trigger err\n");
        goto job_fail;
      }

      usleep(10);
      GPIOWrite(POUT2, 0);

      while (GPIORead(PIN2) == 0)
      {
        start_t = clock();
      }
      while (GPIORead(PIN2) == 1)
      {
        end_t = clock();
      }

      time = (double)(end_t - start_t) / CLOCKS_PER_SEC; // ms
      double distance = (time / 2) * 34000;

      filterValue += distance;

      repeat2++;

      if (repeat2 == REPEAT_MAX)
      {
        int val = (int)filterValue / 5;
        if (val <= 100)
        {
          result.volume = val;
        }
        printf("result volume: %d\n", result.volume);
        filterValue = 0;
        repeat2 = 0;
      }
      usleep(100000);
    } while (1);

    if (-1 == GPIOUnexport(POUT2) || -1 == GPIOUnexport(PIN2))
      goto job_fail;

    return NULL;

  job_fail:
    printf("[SOCKET] socket connection failed\n");
    pthread_exit((void *)-1);
  }
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
  init_socket_server(&server, SERVER_IP, SERVER_PORT);

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

    printf("[SOCKET] write: %d %d %d(%d)\n", result.id, result.note, result.volume, ret);
    usleep(100000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *)-1);
}
