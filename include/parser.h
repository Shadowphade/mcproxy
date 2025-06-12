#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>

struct Address {
    char* ip_addr;
    int port;
};

typedef struct Server {
    char* hostname;
    size_t hostname_n;
    struct Address addr;
} Server;

struct ServerList {
    Server default_server;
    Server* server_list;
    size_t server_list_n;
};

struct ServerList* parseconfig();
struct Address parseipaddress(char* txtaddr);
void printAddress(struct Address input);
void printServer(Server input);
void printServerList(struct ServerList input);

#endif
