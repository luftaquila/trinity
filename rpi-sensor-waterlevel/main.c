#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include "types.h"

void *thread_job_sensor(void *arg);
void *thread_job_socket(void *arg);

int main(void) {
  pthread_t thread_sensor;
  pthread_t thread_socket;

  int tid = pthread_create(&thread_sensor, NULL, thread_job_sensor, NULL);

  if (tid < 0) {
    goto thread_err;
  }

  tid = pthread_create(&thread_socket, NULL, thread_job_socket, NULL);

  return 0;

thread_err:
  printf("Thread creation failed. Terminating...\n");
  return -1;
}

void *thread_job_sensor(void *arg) {
  printf("collecting sensor data...\n");

}

void *thread_job_socket(void *arg) {
  printf("initiating socket...\n");

}
