#include "netwrk/ftp.h"
#include <string.h>
#include <stdlib.h>

static char ftp_data_buff[NK_TCP_MAX_CHUNK_SIZE];

/**
 * @brief Parse the response string into ftp_conn->current_reponse.
 * 
 * @param ftp_conn 
 * @return int 
 */
static int NK_ftp_parse_response(NK_ftp_connection_t *ftp_conn)
{
    char *recv_data;
    int recv_len, tokens, ret;
    NK_string_list_t parsed_list;

    if(!ftp_conn || !ftp_conn->tcp_conn
        || IS_STR_NONE(ftp_conn->tcp_conn->recv_buff)
        || (ftp_conn->tcp_conn->recv_data_len <= 0) )
    {
        return ERR_INVALID_PARAM;
    }
    
    recv_data = ftp_conn->tcp_conn->recv_buff;
    recv_len  = ftp_conn->tcp_conn->recv_data_len;

    // if(strsplit(recv_data, recv_len, " ", &parsed_list) <= 0)
    //     return ERR_PARSE_ERROR;

    ret = sscanf(recv_data, "%d %[^\n]s", &ftp_conn->current_response.code,
                        ftp_conn->current_response.message);
    if(ret <= 0)
        return ERR_PARSE_ERROR;
    
    return 0;
}

int NK_ftp_make_connection(NK_ftp_connection_t *ftp_conn,
                            const char *remote_ip, int16_t remote_port,
                            const char *user_name, const char *password)
{
    int ret = 0;
    NK_ftp_response_t *response;

    if( (!ftp_conn)
        || IS_STR_NONE(remote_ip)
        || (remote_port < 0) )
    {
        return ERR_INVALID_PARAM;
    }

    memset(ftp_conn, 0, sizeof(NK_ftp_connection_t));

    ftp_conn->login_mode = (IS_STR_NONE(user_name))
                            ? LOGIN_MODE_ANONYMOUS : LOGIN_MODE_USER;
    if(ftp_conn->login_mode == LOGIN_MODE_ANONYMOUS) {
        user_name = "anonymous";
        password = "anonymous@example.com";
    }

    /* Make a TCP connection */

    ftp_conn->tcp_conn = calloc(1, sizeof(NK_tcp_connection_t));
    assert_msg( (ftp_conn->tcp_conn != NULL), "ERROR: %s\n", strerror(errno));

    ret = NK_tcp_make_connection(
            ftp_conn->tcp_conn, remote_ip, remote_port, NULL);
    if(ret != 0)
        return ret;
    
    /* Discard the banner message first */
    NK_tcp_recv_all(ftp_conn->tcp_conn);
    printf("%s", ftp_conn->tcp_conn->recv_buff);
    NK_tcp_reset_recv_buff(ftp_conn->tcp_conn);

    response = &ftp_conn->current_response;

    /* Login */

    sprintf(ftp_data_buff, "USER %s\r\n", user_name);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        if(ret == ERR_REMOTE_DEAD)
            NK_tcp_destroy_connection(ftp_conn->tcp_conn);
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_USERNAME_OK)
        return ERR_AUTH_ERROR;
    
    sprintf(ftp_data_buff, "PASS %s\r\n", password);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        if(ret == ERR_REMOTE_DEAD)
            NK_tcp_destroy_connection(ftp_conn->tcp_conn);
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_ACCESS_GRANTED)
        return ERR_AUTH_ERROR;

    return 0;
}

int NK_ftp_change_dir(NK_ftp_connection_t *ftp_conn, const char *dir)
{
    int ret;
    
    if( (!ftp_conn)
        || IS_STR_NONE(dir) )
    {
        return ERR_INVALID_PARAM;
    }

    sprintf(ftp_data_buff, "CWD %s\r\n", dir);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    
    sprintf(ftp_data_buff, "PWD\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }

    return 0;
}

int NK_ftp_get_file(NK_ftp_connection_t *ftp_conn, const char *filename,
                    const char *dir)
{
    int ret;

    if( (!ftp_conn)
        || IS_STR_NONE(filename)
        || IS_STR_NONE(dir) )
    {
        return ERR_INVALID_PARAM;
    }

    if((ret = NK_ftp_change_dir(ftp_conn, dir)) < 0)
        return ret;
    
    sprintf(ftp_data_buff, "TYPE I\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }

    sprintf(ftp_data_buff, "PASV\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    // TODO: Read data port from server and connect to it.
    // Then RETR.
    sprintf(ftp_data_buff, "RETR %s\r\n", filename);
    if((ret = NK_tcp_sendrecv(ftp_conn->tcp_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }

    return 0;
}

/* TODO: FTP receive callback. */

