#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/select.h>
#include <pthread.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/utils.h"

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
                            const char *remote_ip, int remote_port,
                            NK_tcp_recv_callback_t user_cb)
{
    static const char default_local_ip[] = "0.0.0.0";
    struct sockaddr_in remote = {0};
    pthread_t recv_loop_thread;
    int ret;

    if( (!tcp_conn)
        || IS_STR_NONE(remote_ip)
        || (remote_port < 0) )
    {
        return ERR_INVALID_PARAM;
    }
    
    printf("NK_tcp_make_connection(): Making TCP connection to %s:%d\n",
        remote_ip, remote_port);

    /* Making a clean copy of *tcp_conn. */
    memset(tcp_conn, 0, sizeof(NK_tcp_connection_t));

    /* Allocate buffers */
    tcp_conn->recv_buff = (char*)malloc(sizeof(char) * NK_TCP_MAX_CHUNK_SIZE);
    NK_tcp_reset_recv_buff(tcp_conn);

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
    if(!user_cb) {
        printf("%s(): No user callback registered.\n", __FUNCTION__);
        return 0;
    }

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
    tcp_conn->recv_thread_active = 1;

    // pthread_detach(recv_loop_thread);

    return 0;
}

int NK_tcp_destroy_connection(NK_tcp_connection_t *tcp_conn)
{
    void *thread_ret = NULL;

    if( !tcp_conn || (tcp_conn->sock_fd < 0) )
        return ERR_INVALID_PARAM;
    close(tcp_conn->sock_fd);
    tcp_conn->sock_fd = -1;
    if(tcp_conn->recv_thread_active)
        pthread_join(tcp_conn->recv_loop_thread, &thread_ret);
    return 0;
}

void NK_tcp_reset_recv_buff(NK_tcp_connection_t *tcp_conn)
{
    assert(tcp_conn);
    tcp_conn->recv_buff_tail = tcp_conn->recv_buff_head = tcp_conn->recv_buff;
    tcp_conn->recv_data_len = 0;
}

int NK_tcp_recv_fill_buff(NK_tcp_connection_t *tcp_conn)
{
    int ret;
    ssize_t bytes_read, recv_len, max_recv_len;
    char *data_ptr, *recv_buff_end;

    if( !tcp_conn || (tcp_conn->sock_fd < 0) )
        return ERR_INVALID_PARAM;
    
    max_recv_len = NK_TCP_MAX_CHUNK_SIZE;

    /*
     * Clear the internal buffer everytime this function is called.
     */
    NK_tcp_reset_recv_buff(tcp_conn);

    data_ptr = tcp_conn->recv_buff;
    recv_buff_end = tcp_conn->recv_buff + max_recv_len - 1;

    printf("select() wait...\n");
    ret = NK_tcp_wait(tcp_conn->sock_fd, 60, 0);
    if(ret == 0)
        return ERR_READ_TIMEDOUT;
    else if(ret < 0)
        return ERR_CONNECTION_ERROR;
    printf("select() ok. ret = %d.\n", ret);

    /* Try to fill the internal buffer till NK_TCP_MAX_CHUNK_SIZE.  */
    while( ((size_t)data_ptr - (size_t)tcp_conn->recv_buff) < NK_TCP_MAX_CHUNK_SIZE ) {
        recv_len = (ssize_t)(recv_buff_end - data_ptr + 1);
        if(recv_len <= 0) {
            fprintf(stderr, "ERROR: recv_len = %lu.\n", recv_len);
            ret = 0;
            break;
        }
        
        bytes_read = recv(tcp_conn->sock_fd, data_ptr, recv_len, MSG_DONTWAIT);
        
        /* errno == EAGAIN if recv_len > actual_read */
        if(errno == EAGAIN) {
            data_ptr += bytes_read;
            continue;
        }
        /* Keep whatever we got so far. */
        if(bytes_read < 0) {
            fprintf(stderr, "ERROR: recv(%ld): %s.\n", bytes_read, strerror(errno));
            ret = ERR_CONNECTION_ERROR;
            break;
        }
        if(bytes_read == 0) {
            printf("recv(0): %s.\n", strerror(errno));
            ret = 0;
            break;
        }
        data_ptr += bytes_read;
    }

    tcp_conn->recv_buff_tail = data_ptr - 1;
    tcp_conn->recv_data_len = tcp_conn->recv_buff_tail - tcp_conn->recv_buff_head + 1;
    return ret;
}

ssize_t NK_tcp_recv(NK_tcp_connection_t *tcp_conn, char *data, size_t len)
{
    ssize_t bytes_read;

    bytes_read = NK_tcp_wait(tcp_conn->sock_fd, 60, 0);
    if(bytes_read == 0)
        return ERR_READ_TIMEDOUT;
    else if(bytes_read < 0)
        return ERR_CONNECTION_ERROR;
    
    bytes_read = recv(tcp_conn->sock_fd, data, len, 0);
    if (bytes_read < 0)
    {
        fprintf(stderr, "ERROR: recv(%ld): %s.\n", bytes_read, strerror(errno));
        return ERR_CONNECTION_ERROR;
    }
    if (bytes_read == 0) {
        fprintf(stderr, "recv(0): errno = %d, %s.\n", errno, strerror(errno));
        return 0;
    }

    return bytes_read;
}

int NK_tcp_recv_until(NK_tcp_connection_t *tcp_conn, char *data, size_t len,
                      const char *until_token)
{
    ssize_t bytes_read;
    char *until_ptr;

    if( !data || (len < 0) )
        return ERR_INVALID_PARAM;
    
    bytes_read = NK_tcp_recv(tcp_conn, data, len);
    if(bytes_read < 0)
        return (int)bytes_read;
    
    until_ptr = strstr(data, until_token);
    if(!until_ptr)
        return ERR_PARSE_ERROR;
    
    until_ptr += strlen(until_token);
    
    /* Out of bound check.   */
    bytes_read = (ssize_t)((ssize_t)until_ptr - (ssize_t)data);
    if(bytes_read >= len)
        return ERR_MEMORY_ERROR;
    *until_ptr = '\0';
    
    return bytes_read;
}

int NK_tcp_send(NK_tcp_connection_t *tcp_conn, const char *data, ssize_t len)
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

    return len - bytes_left;
}

int NK_tcp_sendrecv(NK_tcp_connection_t *tcp_conn, const char *data, ssize_t len)
{
    const char *data_ptr = data;
    ssize_t bytes_left = len, ret = 0;

    if( !tcp_conn || (tcp_conn->sock_fd < 0)
        || !data || (len < 0) )
        return ERR_INVALID_PARAM;
    
    if((ret = NK_tcp_send(tcp_conn, data, len)) <= 0)
        return ret;

    memset(tcp_conn->recv_buff, 0, NK_TCP_MAX_CHUNK_SIZE);
    ret = recv(tcp_conn->sock_fd, tcp_conn->recv_buff, NK_TCP_MAX_CHUNK_SIZE, 0);
    if(ret < 0)
        return ERR_READ_ERROR;
    else if(ret == 0)
        return ERR_REMOTE_DEAD;
    
    tcp_conn->recv_data_len = ret;
    return ret;
}

long NK_tcp_recvfile(NK_tcp_connection_t *tcp_conn, const char *dest_filename)
{
    FILE *dest_fp;
    long bytes_written = 0;
    int bytes_read = 0, ret;

    if( !tcp_conn || (tcp_conn->sock_fd < 0)
        || IS_STR_NONE(dest_filename) )
        return ERR_INVALID_PARAM;
    
    dest_fp = fopen(dest_filename, "wb");
    if(!dest_fp)
        return ERR_FILE_OPEN_ERROR;
    
    while(1) {
        ret = NK_tcp_recv_fill_buff(tcp_conn);
        if(ret < 0)
            break;
        bytes_read = tcp_conn->recv_data_len;
        if(bytes_read <= 0)
            break;
        bytes_written += fwrite(tcp_conn->recv_buff_head,
                                1, tcp_conn->recv_data_len, dest_fp);
        printf("Received: %ld bytes.\n",
               bytes_written);
    }
    puts("");
    printf("ret: %d, bytes_read: %d\n", ret, bytes_read);
    fclose(dest_fp);
    return bytes_written;
}

int NK_tcp_wait(int sockfd, long sec, long nsec)
{
    int ret;
    fd_set rfd_set;
    struct timespec timeout;

    FD_ZERO(&rfd_set);
    timeout.tv_nsec = nsec;
    timeout.tv_sec = sec;

    FD_SET(sockfd, &rfd_set);
    ret = select(sockfd + 1, &rfd_set, NULL, NULL,
                 (struct timeval *__restrict__)&timeout);
    if (ret < 0) {
        fprintf(stderr, "ERROR: NK_tcp_wait(): select(%d): %s\n",
            ret, strerror(errno));
        return ret;
    }
    if(ret == 0) {
        fprintf(stderr, "ERROR: select() timed out.\n");
        return ret;
    }
    return FD_ISSET(sockfd, &rfd_set);
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

    while (NK_tcp_wait(tcp_conn->sock_fd, 90, 0) > 0) {
        tcp_conn->recv_data_len = recv(tcp_conn->sock_fd,
                                       tcp_conn->recv_buff,
                                       NK_TCP_MAX_CHUNK_SIZE, 0);
        if (tcp_conn->recv_data_len == 0) {
            printf("Connection closed by remote host.\n");
            return NULL;
        }
        else if (tcp_conn->recv_data_len < 0) {
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
