#ifndef __TYPES_H
#define __TYPES_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

/* socket server configuration */
#if SERVER_MOCK
#define SERVER_IP "127.0.0.1"
#else
#define SERVER_IP "192.168.33.70"
#endif

#define SERVER_PORT  5000

/* socket communication payload */
typedef struct {
  int id;
  int note;
  int volume;
} payload_t;

typedef enum {
  ID_BUTTONS = 0,
  ID_ULTRASONIC,
  ID_WATERLEVEL,
  ID_ACCEL
} socket_id_t;

/**************************************************************************
 * initialize socket server info
 */
int init_socket_server(struct sockaddr_in *server, const char *addr, int port) {
  memset(server, 0, sizeof(struct sockaddr_in));

  server->sin_family = AF_INET;
  server->sin_addr.s_addr = inet_addr(addr);
  server->sin_port = htons(port);

  return 0;
}

/* socket server */


#endif /* __TYPES_H */
