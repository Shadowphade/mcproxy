#ifndef SERVER_H
#define SERVER_H
#include <stdio.h>
#include "parser.h"

struct Connection {
    int mc_client_fd;
    int mc_server_fd;
};

int startserver(int port, struct ServerList* server_list);
void* acceptConection(void* args);
int passconnection(int clientFD, struct Address outgoingaddress, char* msg, int msg_size);
void* handlemc_client(void* args);
void* handlemc_server(void* args);

#endif
