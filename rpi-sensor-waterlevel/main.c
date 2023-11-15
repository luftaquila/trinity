#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

/* MOCKUP SERVER configuration */
#define SERVER_MOCK 1

#include "types.h"

/* global variables */
int result = 0;

/* function prototypes */
void *thread_job_sensor(void *arg);
void *thread_job_socket(void *arg);


/**************************************************************************
 * main function
 */
int main(void) {
  pthread_t thread_sensor;
  pthread_t thread_socket;

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
  printf("[SENSOR] collecting sensor data...\n");

  while (1) {
    result++; // TODO: sensor measurement here
    usleep(10000);
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
  init_socket_server(&server, SERVER_IP, SERVER_PORT_3);

  printf("[SOCKET] waiting for server...\n");
  ret = connect(sock, (struct sockaddr*)&server, sizeof(server));

  if (ret < 0) {
    goto socket_fail;
  }

  while (1) {
    ret = write(sock, &result, sizeof(result));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    printf("[SOCKET] write: %d(%d)\n", result, ret);
    usleep(10000);
  }

socket_fail:
  printf("[SOCKET] socket connection failed\n");
  pthread_exit((void *) -1);
}
