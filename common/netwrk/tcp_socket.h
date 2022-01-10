#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "netwrk/netwrk.h"
#include "netwrk/ip.h"

#define NK_TCP_MAX_DATA_SIZE		2049

typedef struct _NK_tcp_connection_t NK_tcp_connection_t;

/**
 * @brief Callback called whenever a data is received from remote.
 * 
 */
typedef int (*NK_tcp_recv_callback_t)(NK_tcp_connection_t *tcp_conn, void *user_context);

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

    /* Receive loop thread handle. */
    pthread_t recv_loop_thread;
    
    /* User callback to handle received data. */
    NK_tcp_recv_callback_t recv_user_cb;

    /* User/application specific context data. */
    void *user_context;
};

/**
 * @brief Closes the connection to the server on the socket descriptor
 * conn_sock_fd.
 * 
 * @param conn_sock_fd Socket descriptor of the connection to be closed. 
 * @return int 0 on successful. -1 on error.
 */
int NK_tcp_destroy_connection(NK_tcp_connection_t *tcp_conn);

/**
 * @brief Connects to the TCP server at @remote_ip and @remote_port and
 * populates the context in @tcp_conn.
 * 
 * @param tcp_conn 
 * @param remote_ip 
 * @param remote_port 
 * @return int 
 */
int NK_tcp_make_connection(NK_tcp_connection_t *tcp_conn,
                            const char* remote_ip, int16_t remote_port,
                            NK_tcp_recv_callback_t user_cb);


