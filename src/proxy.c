#include "../include/proxy.h"
#include "../include/datatype.h"
#include "../include/parser.h"
#include "../include/net_handle.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>


struct ServerList* serverlist;

int startproxy(int port, struct ServerList* server_list) {

    serverlist = server_list;

    struct sockaddr_in address;
    socklen_t addrlen = 0;
    int proxyFD = create_proxy_socket(port, &address, &addrlen);

    printf("Server List Length: %zu",  serverlist->server_list_n);

    int connection = 1;
    while(connection) {
        int clientFD = accept(proxyFD, (struct sockaddr*)&address, &addrlen);
        if(clientFD < 0) {
            printf("ERROR ACCEPTING CONNECTION\n");
            return 0;
        }
        //struct MinecraftRequest req = acceptConection(clientFD);
        pthread_t newthread_id;
        pthread_create(&newthread_id, NULL, acceptConection, &clientFD);
    }

    return 1;
};
void* acceptConection(void* args) {
    char buffer[2048] = { '\0' };

    int clientFD = *(int*) args;

    int readbytes = read(clientFD, buffer, 2047);
    struct MinecraftRequestHeader routereq = parserequest(buffer, 2048);
    printf("Requested Domain: %s\n", routereq.name);
    printf("Server List Length: %zu\n",  serverlist->server_list_n);
    for(int i = 0; i < serverlist->server_list_n; i++) { // Implement a better strcmp method so that things are less bad
        if (strcmp(serverlist->server_list[i].hostname, routereq.name) == 0) {
            int err = passconnection(clientFD, serverlist->server_list[i].addr, buffer, readbytes);
            if (err == 0) {return NULL;}
            break;
        }
    }
    printf("REQUESTED DOMAIN NOT FOUND ROUTING TO DEFAULT\n");
    int err = passconnection(clientFD, serverlist->default_server.addr, buffer, readbytes);
    if (err == 0) {
        printf("ERROR PASSING CONNECTION\n");
        return NULL;
    }

    return NULL;
}

int passconnection(int clientFD, struct Address outgoingaddress, char* msg, int msg_size) {
    char buffer[2048] = { '\0' };

    int mc_server_fd = create_server_socket(outgoingaddress);

    if(mc_server_fd == 0) {
        return 1;
    }

    struct VarInt size = readvarint(msg);
    printf("Writing: \n");
    fwrite(msg, 1, 2048, stdout);
    printf("\n\n");
    send(mc_server_fd, msg, size.num, 0);

    struct Connection *conn = malloc(sizeof(struct Connection));
    conn->mc_server_fd = mc_server_fd;
    conn->mc_client_fd = clientFD;

    printf("Connection on main thread: %d, %d", conn->mc_client_fd, conn->mc_server_fd);


    //Do proxy stuff here!!


    return 1;
}
