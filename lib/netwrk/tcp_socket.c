#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/select.h>
#include <pthread.h>
#include "tcp_socket.h"

static void* NK_tcp_recv_loop(void *tcp_conn);

static int NK_tcp_create_socket()
{
    int sock_fd = -1;

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock_fd < 0)
        perror("socket");
    
    return sock_fd;
}

int NK_tcp_make_connection(NK_tcp_connection_t *tcp_conn,
                            const char* remote_ip, int16_t remote_port,
                            NK_tcp_recv_callback_t user_cb)
{
    static const char default_local_ip[] = "0.0.0.0";
    struct sockaddr_in remote = {0};
    pthread_t recv_loop_thread;
    int ret;

    if( (!tcp_conn)
        || (!remote_ip) || !strlen(remote_ip)
        || (remote_port < 0) )
    {
        return ERR_INVALID_PARAM;
    }
    
    // Clean copy of *tcp_conn.
    memset(tcp_conn, 0, sizeof(NK_tcp_connection_t));
    
    /*
     * Setup TCP socket and connection.
     */

    tcp_conn->sock_fd = NK_tcp_create_socket();
    if(tcp_conn->sock_fd < 0)
        return ERR_INVALID_SOCKET;

    strncpy(tcp_conn->remote_ip, remote_ip,
            NK_MIN(strlen(remote_ip), NK_MAX_IPV4_LEN));
    strncpy(tcp_conn->local_ip, default_local_ip,
            NK_MIN(strlen(default_local_ip), NK_MAX_IPV4_LEN));
    tcp_conn->remote_port = remote_port;
    tcp_conn->local_port = 0;

    remote.sin_family = AF_INET;
    if(inet_pton(AF_INET, remote_ip, (void*)&remote.sin_addr) <= 0) {
        perror("inet_aton");
        return ERR_INVALID_ADDRESS;
    }
    remote.sin_port = htons((uint16_t)remote_port);
    
    if(connect(tcp_conn->sock_fd, (struct sockaddr*)&remote,
                sizeof(struct sockaddr_in)) < 0)
        return ERR_CONNECTION_ERROR;
    
    /*
     * If user_cb is not provided then no need to start the
     * receive thread.
     */
    if(!user_cb)
        return 0;

    /*
     * Start the receive loop function in a thread.
     */
    tcp_conn->recv_user_cb = user_cb;
    ret = pthread_create(&recv_loop_thread, NULL, NK_tcp_recv_loop, (void*)tcp_conn);
    if(ret != 0) {
        fprintf(stderr, "ERROR: Failed to create receive loop thread.\n");
        return ERR_PTHREAD_ERROR;
    }
    tcp_conn->recv_loop_thread = recv_loop_thread;

    // pthread_detach(recv_loop_thread);

    return 0;
}

int NK_tcp_destroy_connection(NK_tcp_connection_t *tcp_conn)
{
    void *thread_ret = NULL;

    if( !tcp_conn || (tcp_conn->sock_fd < 0) )
        return ERR_INVALID_PARAM;
    close(tcp_conn->sock_fd);
    pthread_join(tcp_conn->recv_loop_thread, &thread_ret);
    return 0;
}

/*
 * Low-level API.
 * Copies received data from kernel to app's internal buffer
 * tcp_conn->recv_data_buff. Tries to fill the whole buffer.
 */
static int NK_tcp_recv_all(NK_tcp_connection_t *tcp_conn)
{
    int ret;
    size_t recv_len, recv_buff_size;
    char *data_ptr, *recv_buff_end;

    if( !tcp_conn || (tcp_conn->sock_fd < 0) )
        return ERR_INVALID_PARAM;
    
    recv_buff_size = sizeof(tcp_conn->recv_buff);
    
    /*
     * Buffer is actually not full. We are just not allowing
     * receival of more bytes if tail is at the end.
     * ERR_BUFFER_FULL means that there is still data to be
     * processed.
     */
    if( tcp_conn->recv_buff_tail == (tcp_conn->recv_buff + recv_buff_size - 1) ) {
        if(tcp_conn->recv_buff_tail > tcp_conn->recv_buff_head)
            return ERR_BUFFER_FULL;
        tcp_conn->recv_buff_tail = tcp_conn->recv_buff_head = tcp_conn->recv_data_len = 0;
    }

    if(!NK_tcp_wait(tcp_conn, 90, 0))
        return ERR_READ_TIMEDOUT;
    
    data_ptr = tcp_conn->recv_buff_tail;
    recv_buff_end = tcp_conn->recv_buff + recv_buff_size;

    while( (data_ptr - tcp_conn->recv_buff_head) < NK_TCP_MAX_CHUNK_SIZE ) {
        recv_len = (size_t)(recv_buff_end - data_ptr);
        if(!recv_len)
            break;
        ret = recv(tcp_conn->sock_fd, data_ptr, recv_len, MSG_DONTWAIT);
        
        // Keep whatever we got so far.
        if(ret <= 0)
            break;
        
        data_ptr += ret;
    }

    tcp_conn->recv_buff_tail = data_ptr;
    tcp_conn->recv_data_len = tcp_conn->recv_buff_tail - tcp_conn->recv_buff_head;
    return tcp_conn->recv_data_len;
}

int NK_tcp_recv_until(NK_tcp_connection_t *tcp_conn, char *data, ssize_t len,
                      const char *until_token)
{
    char *data_ptr = data;
    ssize_t ret = 0;

    if( !data || (len < 0) )
        return ERR_INVALID_PARAM;
    
    ret = NK_tcp_recv_all(tcp_conn);
    if( (ret < 0) && (ret != ERR_BUFFER_FULL))
        return ret;

    if(!memmem(tcp_conn->recv_buff_head, tcp_conn->recv_data_len,
               until_token, strlen(until_token)))
        return ERR_PARSE_ERROR;
    tcp_conn->recv_buff_head = (char*)memccpy(data, tcp_conn->recv_buff_head, until_token,
                                NK_MIN(len, (tcp_conn->recv_data_len)));
    tcp_conn->recv_data_len = tcp_conn->recv_buff_tail - tcp_conn->recv_buff_head;

    return ret;
}

int NK_tcp_sendrecv(NK_tcp_connection_t *tcp_conn, const char *data, ssize_t len)
{
    const char *data_ptr = data;
    ssize_t bytes_left = len, ret = 0;

    if( !tcp_conn || (tcp_conn->sock_fd < 0)
        || !data || (len < 0) )
        return ERR_INVALID_PARAM;
    
    while(bytes_left > 0) {
        ret = send(tcp_conn->sock_fd, data_ptr, len, 0);
        if(ret <= 0)
            return ERR_WRITE_ERROR;
        data_ptr += ret;
        bytes_left -= ret;
    }

    memset(tcp_conn->recv_buff, 0, sizeof(tcp_conn->recv_buff));
    ret = recv(tcp_conn->sock_fd, tcp_conn->recv_buff, NK_TCP_MAX_CHUNK_SIZE, 0);
    if(ret < 0)
        return ERR_READ_ERROR;
    else if(ret == 0)
        return ERR_REMOTE_DEAD;
    
    tcp_conn->recv_data_len = ret;
    return ret;
}

int NK_tcp_wait(NK_tcp_connection_t *tcp_conn, long sec, long nsec)
{
    int ret;
    fd_set rfd_set;
    struct timespec timeout;

    FD_ZERO(&rfd_set);
    timeout.tv_nsec = nsec;
    timeout.tv_sec = sec; // 90 sec timeout for now.

    FD_SET(tcp_conn->sock_fd, &rfd_set);
    ret = select(tcp_conn->sock_fd + 1, &rfd_set, NULL, NULL,
                 (struct timeval *__restrict__)&timeout);
    if (ret == 0) {
        fprintf(stderr, "ERROR: NK_tcp_wait(): select() timed out.\n");
        return ret;
    }
    return FD_ISSET(tcp_conn->sock_fd, &rfd_set);
}

/**
 * @brief A looper function that keeps on reading data from the TCP socket and 
 * invokes the user callback on each data receival. This function is run as a
 * separate thread. 
 */
static void* NK_tcp_recv_loop(void *arg)
{
    int ret = 0;
    NK_tcp_connection_t *tcp_conn = NULL;

    tcp_conn = (NK_tcp_connection_t*)arg;
    if( !tcp_conn || (tcp_conn->sock_fd < 0) ) {
        fprintf(stderr, "ERROR: NK_tcp_recv_loop(): Invalid parameter.\n");
        assert(tcp_conn->sock_fd > -1);
    }

    // Let the main thread reach shell prompt.
    sleep(1);

    while (NK_tcp_wait(tcp_conn, 90, 0))
    {
        tcp_conn->recv_data_len = recv(tcp_conn->sock_fd,
                                       tcp_conn->recv_buff,
                                       NK_TCP_MAX_CHUNK_SIZE, 0);
        if (tcp_conn->recv_data_len == 0)
        {
            printf("Connection closed by remote host.\n");
            return NULL;
        }
        else if (tcp_conn->recv_data_len < 0)
        {
            fprintf(stderr, "ERROR: NK_tcp_recv_loop(): recv() returned -1.\n");
            continue;
        }

        // User callback.
        if (tcp_conn->recv_user_cb)
            tcp_conn->recv_user_cb(tcp_conn, tcp_conn->user_context);
        else
            printf("??? User callback not set ???\nData:\n%s\n",
                   tcp_conn->recv_buff);
    }

    return NULL;
}
