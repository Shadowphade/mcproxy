#include "../include/server.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/datatype.h"
#include "../include/parser.h"

struct ServerList* serverlist;

int startserver(int port, struct ServerList* server_list) {
    int opt = 1;
    serverlist = server_list;
    int serverFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    if (serverFD < 0) {
        printf("ERROR CREATING SOCKET\n");
        return 0;
    }

    int opterr = setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if(opterr) {
        printf("ERROR SETTING SOCKET OPTIONS\n");
        return 0;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int binderr = bind(serverFD, (struct sockaddr*)&address, sizeof(address));
    if (binderr < 0) {
        perror("bind");
        printf("ERROR BINDING SOCKET\n");
        return 0;
    }

    int listenerr = listen(serverFD, 5);
    if (listenerr < 0) {
        printf("ERROR LISTENING\n");
        return 0;
    }

    printf("Server List Length: %zu",  serverlist->server_list_n);

    int connection = 1;

    while(connection) {
        int clientFD = accept(serverFD, (struct sockaddr*)&address, &addrlen);
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
    for(int i = 0; i < serverlist->server_list_n; i++) {
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

    struct sockaddr_in mc_server_addr;

    int mc_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(mc_server_fd < 0) {
        printf("ERROR CREATING SOCKET");
        return 0;
    }

    mc_server_addr.sin_family = AF_INET;
    mc_server_addr.sin_port = htons(outgoingaddress.port);
    printf("connecting to: %s\n", outgoingaddress.ip_addr);
    int addrerr = inet_pton(AF_INET, outgoingaddress.ip_addr, &mc_server_addr.sin_addr);

    if (addrerr <= 0) {
        printf("ERROR: INVALID ADDRESS");
        return 0;
    }

    int status = connect(mc_server_fd, (struct sockaddr*)&mc_server_addr, sizeof(mc_server_addr));

    if (status < 0) {
        printf("CONNECTION FAILED");
        return 0;
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

    pthread_t server_thread;
    pthread_t client_thread;

    pthread_create(&client_thread, NULL, handlemc_client, conn);
    pthread_create(&server_thread, NULL, handlemc_server, conn);


    return 1;
}

void* handlemc_client(void* args) {
    printf("Started Client thread\n");
    struct Connection* conn_client = (struct Connection*) args;
    char buffer[2048] = { '\0' };
    printf("Connection on client thread: %d, %d", conn_client->mc_client_fd, conn_client->mc_server_fd);
    int readbytes = 1;
    while(readbytes != 0) {
        int readbytes = read(conn_client->mc_client_fd, buffer, 2047);
        fwrite(buffer, 1, 2048, stdout);
        //printf("\n\n");
        write(conn_client->mc_server_fd, buffer, readbytes);
    }
    return NULL;
}

void* handlemc_server(void* args) {
    printf("Started Server thread\n");
    struct Connection* conn_server = (struct Connection*) args;
    char buffer[2048] = { '\0' };
    printf("Connection on server thread: %d, %d", conn_server->mc_client_fd, conn_server->mc_server_fd);
    int readbytes = 1;
    while(readbytes != 0) {
        int readbytes = read(conn_server->mc_server_fd, buffer, 2047);
        fwrite(buffer, 1, 2048, stdout);
        //printf("\n\n");
        write(conn_server->mc_client_fd, buffer, readbytes);
    }
    return NULL;
}

