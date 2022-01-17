#pragma once

#include "netwrk/netwrk.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "netwrk/ip.h"

#define NK_TCP_MAX_DATA_SIZE		2049

typedef struct _NK_tcp_connection_t NK_tcp_connection_t;

/**
 * @brief Callback called whenever a data is received from remote.
 * 
 */
typedef int (*NK_tcp_recv_callback_t)(NK_tcp_connection_t *tcp_conn,
                void *user_context);

/**
 * @brief The context of a single TCP connection.
 * 
 */
struct _NK_tcp_connection_t
{
    int sock_fd;
    int16_t local_port, remote_port;
    char local_ip[NK_MAX_IPV4_LEN], remote_ip[NK_MAX_IPV4_LEN];
    
    /*
     * Received data is stored raw in this buffer, along with length.
     */
    int recv_data_len;
    char recv_buff[NK_TCP_MAX_DATA_SIZE];

    /* 
     * Receive loop thread handle.
     * Calls @recv_user_cb when a data received.
     */
    pthread_t recv_loop_thread;
    NK_tcp_recv_callback_t recv_user_cb;
    void *user_context;
};

/**
 * @brief Connects to the TCP server at @remote_ip and @remote_port and
 *   populates the context in @tcp_conn.
 * 
 * @param tcp_conn 
 * @param remote_ip 
 * @param remote_port 
 * @return int 
 */
int NK_tcp_make_connection(NK_tcp_connection_t *tcp_conn,
                            const char* remote_ip, int16_t remote_port,
                            NK_tcp_recv_callback_t user_cb);

/**
 * @brief Closes the connection to the server on the socket descriptor
 *   conn_sock_fd.
 * 
 * @param conn_sock_fd Socket descriptor of the connection to be closed. 
 * @return int 0 on successful. -1 on error.
 */
int NK_tcp_destroy_connection(NK_tcp_connection_t *tcp_conn);

/**
 * @brief Send and then receive data synchronously from the remote host for
 *   the connection @tcp_conn. It is made sure that the entire @len bytes
 *   of data in @data is sent.
 * 
 * @param tcp_conn Connection that determines the hosts.
 * @param data Data bytes to be sent.
 * @param len  Byte size of the data.
 * @return int Number of bytes received from remote host.
 */
int NK_tcp_sendrecv(NK_tcp_connection_t *tcp_conn, const char *data, ssize_t len);

