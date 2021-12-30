#include <stdio.h>
#include <stdlib.h>
#include "tcp_socket.h"

int NK_tcp_create_socket()
{
    int sock_fd = -1;

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(sock_fd < 0)
        perror("socket");
    
    return sock_fd;
}

int NK_tcp_connect(int sock_fd, const char* ip, int port)
{
    struct sockaddr_in remote = {0};
    
    if( (sock_fd < 0) || (!ip) || (port < 0) )
        return -1;
    
    remote.sin_family = AF_INET;
    if(inet_pton(AF_INET, ip, (void*)&remote.sin_addr) <= 0) {
        perror("inet_aton");
        return -1;
    }
    remote.sin_port = htons((uint16_t)port);
    
    return connect(sock_fd, (struct sockaddr*)&remote, sizeof(struct sockaddr_in));
}

int NK_tcp_disconnect(int conn_sock_fd)
{
    if(conn_sock_fd < 0)
        return -1;
    close(conn_sock_fd);
    return 0;
}

