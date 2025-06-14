#include "../include/net_handle.h"

#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

// Netcode boilerplate sectioned off to perhaps add more options for other os's network stacks
int create_proxy_socket(int port, struct sockaddr_in* address, socklen_t *addrlen) {
    int opt = 1;
    int proxyFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    *addrlen = sizeof(*address);
    printf("Size of addr: %d\n", *addrlen);
    if (proxyFD < 0) {
        printf("ERROR CREATING SOCKET\n");
        return 0;
    }

    int opterr = setsockopt(proxyFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if(opterr) {
        printf("ERROR SETTING SOCKET OPTIONS\n");
        return 0;
    }

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    int binderr = bind(proxyFD, (struct sockaddr*)address, *addrlen);
    if (binderr < 0) {
        perror("bind");
        printf("ERROR BINDING SOCKET\n");
        return 0;
    }

    int listenerr = listen(proxyFD, 5);
    if (listenerr < 0) {
        printf("ERROR LISTENING\n");
        return 0;
    }

    return proxyFD;
}
int create_server_socket(struct Address addr) {

    struct sockaddr_in mc_server_addr;

    int mc_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(mc_server_fd < 0) {
        printf("ERROR CREATING SOCKET");
        return 0;
    }

    mc_server_addr.sin_family = AF_INET;
    mc_server_addr.sin_port = htons(addr.port);
    printf("connecting to: %s\n", addr.ip_addr);
    int addrerr = inet_pton(AF_INET, addr.ip_addr, &mc_server_addr.sin_addr);

    if (addrerr <= 0) {
        printf("ERROR: INVALID ADDRESS");
        return 0;
    }

    int status = connect(mc_server_fd, (struct sockaddr*)&mc_server_addr, sizeof(mc_server_addr));

    if (status < 0) {
        printf("CONNECTION FAILED");
        return 0;
    }

    return mc_server_fd;
}


