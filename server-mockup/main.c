#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "types.h"

void *thread_job_server_mock(void *arg);
void *thread_job_display_mock(void *arg);

int main() {
  int port = SERVER_PORT;
  pthread_t thread;

  int display_port = SERVER_PORT + 1;
  pthread_t thread_display;

  pthread_create(&thread, NULL, thread_job_server_mock, &port);
  pthread_create(&thread_display, NULL, thread_job_display_mock, &display_port);

  pthread_join(thread, NULL);

  return 0;
}

void *thread_job_server_mock(void *arg) {
  int port = *(int *)arg;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  bind(sock, (struct sockaddr *)&server, sizeof(server));

  listen(sock, 1);
  printf("[SERVER] listening on %d\n", port);

  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  int fd = accept(sock, (struct sockaddr *)&client, &len);
  printf("[SERVER] socket establishment on %d\n", port);

  payload_t rcv;

  while (1) {
    int ret = read(fd, &rcv, sizeof(rcv));

    if (ret < 0) {
      printf("[SOCKET] read failed: %s\n", strerror(errno));
    }

    printf("[SERVER] rcv: { id: %d, note: %d, volume: %d }\n", rcv.id, rcv.note, rcv.volume);
    usleep(10000);
  }

  return 0;
}

void *thread_job_display_mock(void *arg) {
  int port = *(int *)arg;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  bind(sock, (struct sockaddr *)&server, sizeof(server));

  listen(sock, 1);
  printf("[DISPLAY] listening on %d\n", port);

  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  int fd = accept(sock, (struct sockaddr *)&client, &len);
  printf("[DISPLAY] socket establishment on %d\n", port);

  int arr[4];

  while (1) {
    for (int i = 0; i < 4; i++) {
      arr[i] = rand() % 160;
    }

    int ret = write(fd, arr, sizeof(arr));

    if (ret < 0) {
      printf("[SOCKET] write failed: %s\n", strerror(errno));
    }

    usleep(20000);
  }

  return 0;
}
