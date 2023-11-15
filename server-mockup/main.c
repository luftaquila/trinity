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
  int ports[SERVER_PORT_CNT] = { SERVER_PORT_1, SERVER_PORT_2, SERVER_PORT_3 };
  pthread_t threads[SERVER_PORT_CNT];

  for (int i = 0; i < SERVER_PORT_CNT; i++) {
    pthread_create(&threads[i], NULL, thread_job_server_mock, &ports[i]);
  }

  pthread_join(threads[0], NULL);

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

  bind(sock, (struct sockaddr*)&server, sizeof(server));

  listen(sock, 1);
  printf("[SERVER] listening on %d\n", port);

  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  int fd = accept(sock, (struct sockaddr*)&client, &len);
  printf("[SERVER] socket establishment on %d\n", port);

  int rcv;
  while (1) {
    int ret = read(fd, &rcv, sizeof(rcv));

    if (ret < 0) {
      printf("[SOCKET] read failed: %s\n", strerror(errno));
    }

    printf("[SERVER] [%d] rcv: %d\n", port, rcv);
    usleep(10000);
  }

  return 0;
}
