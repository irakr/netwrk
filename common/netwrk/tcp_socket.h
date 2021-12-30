#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * @brief Creates a TCP socket and returns the socket descriptor.
 * 
 * @return int Socket descriptor.
 */
int NK_tcp_create_socket();

/**
 * @brief Sends a connect request and blocks. Returns a socket descriptor
 * of the newly established connection, if successful.
 * 
 * @param sock_fd Main TCP socket returned by NK_tcp_create_socket().
 * @param ip IP address of the target server.
 * @param port Port of the target server.
 * @return int New connection's socket descriptor. -1 on error.
 */
int NK_tcp_connect_server(int sock_fd, const char* ip, int port);

/**
 * @brief Closes the connection to the server on the socket descriptor
 * conn_sock_fd.
 * 
 * @param conn_sock_fd Socket descriptor of the connection to be closed. 
 * @return int 0 on successful. -1 on error.
 */
int NK_tcp_disconnect_server(int conn_sock_fd);

