#ifndef NET_HANDLE_H
#define NET_HANDLE_H
#include "parser.h"
#include <unistd.h>
#include <netinet/in.h>


int create_proxy_socket(int port, struct sockaddr_in* address, socklen_t *addrlen);
int create_server_socket(struct Address addr);
int create_client_socket(struct Address addr);

void set_socket_nonblock(int fd);

#endif
