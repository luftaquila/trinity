#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "types.h"

void *thread_job_server_mock(void *arg);

int main() {
  int port = SERVER_PORT;
  pthread_t thread;

  int port1 = 5001;
  pthread_t thread1;

  pthread_create(&thread, NULL, thread_job_server_mock, &port);
  pthread_create(&thread1, NULL, thread_job_server_mock, &port1);

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
