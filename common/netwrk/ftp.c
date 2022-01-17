#include "netwrk/ftp.h"
#include <string.h>
#include <stdlib.h>

static char data_buff[NK_TCP_MAX_DATA_SIZE];

/**
 * @brief Parse the response string into ftp_conn->current_reponse.
 * 
 * @param ftp_conn 
 * @return int 
 */
static int NK_ftp_parse_response(NK_ftp_connection_t *ftp_conn)
{
    char *recv_data;
    int recv_len;

    if(!ftp_conn || !ftp_conn->tcp_conn)
        return ERR_INVALID_PARAM;
    
    recv_data = ftp_conn->tcp_conn->recv_buff;
    recv_len  = ftp_conn->tcp_conn->recv_data_len;

    if(!recv_data || !strlen(recv_data) || (recv_len <= 0) )
        return ERR_INVALID_PARAM;

    

    return 0;
}

int NK_ftp_make_connection(NK_ftp_connection_t *ftp_conn,
                            const char *remote_ip, int16_t remote_port,
                            const char *user_name, const char *password)
{
    int ret = 0;

    if( (!ftp_conn)
        || (!remote_ip) || !strlen(remote_ip)
        || (remote_port < 0)
        || (!password) || !strlen(password) )
    {
        return ERR_INVALID_PARAM;
    }

    memset(ftp_conn, 0, sizeof(NK_ftp_connection_t));

    ftp_conn->login_mode = (!remote_ip || !strlen(remote_ip))
                            ? LOGIN_MODE_ANONYMOUS : LOGIN_MODE_USER;
    if(ftp_conn->login_mode == LOGIN_MODE_ANONYMOUS) { // TODO
        return ERR_NOT_IMPLEMENTED;
    }

    /* Make a TCP connection */

    ftp_conn->tcp_conn = calloc(1, sizeof(NK_tcp_connection_t));
    assert_msg( (ftp_conn->tcp_conn != NULL), "ERROR: %s\n", strerror(errno));

    ret = NK_tcp_make_connection(
            ftp_conn->tcp_conn, remote_ip, remote_port, NULL);
    if(ret != 0)
        return ret;
    
    /* Login */

    sprintf(data_buff, "USER %s\n", user_name);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, data_buff, strlen(data_buff)))
                    < 0)
    {
        NK_tcp_destroy_connection(ftp_conn->tcp_conn);
        return ret;
    }

    sprintf(data_buff, "PASS %s\n", password);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, data_buff, strlen(data_buff)))
                    < 0)
    {
        NK_tcp_destroy_connection(ftp_conn->tcp_conn);
        return ret;
    }

    return 0;
}

/* TODO: FTP receive callback. */

