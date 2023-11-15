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
#define SERVER_IP "192.168.31.2"
#endif

#define SERVER_PORT_CNT 3
#define SERVER_PORT_1  5001
#define SERVER_PORT_2  5002
#define SERVER_PORT_3  5003

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
