#pragma once

#include "netwrk/netwrk.h"
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "netwrk/inet.h"

#define NK_TCP_MAX_CHUNK_SIZE		((size_t)(65536))

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
    int local_port, remote_port;
    char local_ip[NK_MAX_IPV4_LEN], remote_ip[NK_MAX_IPV4_LEN];
    
    /*
     * Received data is stored raw in this buffer.
     * Currently this buffer is maintained as a simple queue.
     * @recv_buff: Base pointer of the buffer.
     *   NK_TCP_MAX_CHUNK_SIZE bytes are pre-allocated on the heap.
     * @recv_buff_head: Base pointer to the unprocessed data.
     * @recv_buff_tail: End pointer to the unprocessed data.
     * 
     * TODO: Make this a ring buffer.
     */
    int recv_data_len;
    char *recv_buff;
    char *recv_buff_head;
    char *recv_buff_tail;

    /* 
     * Receive loop thread handle.
     * Calls @recv_user_cb when a data received.
     */
    pthread_t recv_loop_thread;
    uint8_t recv_thread_active;
    NK_tcp_recv_callback_t recv_user_cb;
    void *user_context;
};

/**
 * @brief Connects to the TCP server at @remote_ip and @remote_port and
 *   populates the context in @tcp_conn. If @user_cb is provided then
 *   a receive thread will be created and user_cb callback will called
 *   for each TCP message received.
 * 
 * @param tcp_conn 
 * @param remote_ip 
 * @param remote_port 
 * @return int 
 */
int NK_tcp_make_connection(NK_tcp_connection_t *tcp_conn,
                            const char *remote_ip, int remote_port,
                            NK_tcp_recv_callback_t user_cb);

/**
 * @brief Closes the connection to the server on the socket descriptor
 *   conn_sock_fd.
 * 
 * @param conn_sock_fd Socket descriptor of the connection to be closed. 
 * @return int 0 on successful. -1 on error.
 */
int NK_tcp_destroy_connection(NK_tcp_connection_t *tcp_conn);

void NK_tcp_reset_recv_buff(NK_tcp_connection_t *tcp_conn);

/**
 * @brief Receive a single message.
 * 
 * @param tcp_conn 
 * @param data 
 * @param len 
 * @return ssize_t No. of bytes read. ERR_XXX on error. 
 */
ssize_t NK_tcp_recv(NK_tcp_connection_t *tcp_conn, char *data, size_t len);

/**
 * @brief Copies all received data from kernel to app's internal buffer
 *   tcp_conn->recv_data_buff. Tries to fill the whole buffer.
 *   Use this function only when reading data freshly and throughout.
 *   This is used by NK_tcp_recvfile() like functions.
 */
int NK_tcp_recv_fill_buff(NK_tcp_connection_t *tcp_conn);

/**
 * @brief Extract received data stream until the token @until_token.
 *   It makes use of the head and tail pointer of the internal buffer
 *   so do not mix it up with NK_tcp_recv_fill_buff().
 * @param tcp_conn 
 * @param data 
 * @param len 
 * @param until_token 
 * @return int 
 */
int NK_tcp_recv_until(NK_tcp_connection_t *tcp_conn, char *data, size_t len,
                      const char *until_token);

/**
 * @brief Send and then receive data synchronously from the remote host for
 *   the connection @tcp_conn. It is made sure that the entire @len bytes
 *   of data in @data is sent.
 * @note Do not use this function if the parameter user_cb was NULL to the
 *   function NK_tcp_make_connection().
 * 
 * @param tcp_conn Connection that determines the hosts.
 * @param data Data bytes to be sent.
 * @param len  Byte size of the data.
 * @return int Number of bytes received from remote host.
 */
int NK_tcp_sendrecv(NK_tcp_connection_t *tcp_conn, const char *data, ssize_t len);

/**
 * @brief Receive file data. The received data is stored in the file @dest_filename.
 * 
 * @param tcp_conn 
 * @param dest_filename 
 * @return long Total bytes received for the file. 
 */
long NK_tcp_recvfile(NK_tcp_connection_t *tcp_conn, const char *dest_filename);

/**
 * @brief Wait for something on the socket @sockfd for
 *   @sec seconds and @nsec nanoseconds.
 *   Uses select().
 * 
 * @return Return 1(something's up) or 0(nothing, timed out)
 */
int NK_tcp_wait(int sockfd, long sec, long nsec);

