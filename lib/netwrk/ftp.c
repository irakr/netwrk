#include "netwrk/ftp.h"

static char ftp_data_buff[NK_TCP_MAX_CHUNK_SIZE];

int NK_ftp_parse_url(const char *ftp_url_str, NK_ftp_url_t *ftp_url)
{
    char *p_substr = ftp_url_str, *p_substr1, *p_substr2;
    int ret;

    if( IS_STR_NONE(ftp_url_str) || !ftp_url)
        return ERR_INVALID_PARAM;
    
    memset(ftp_url, 0, sizeof(NK_ftp_url_t));
    
    /* Check 'ftp://' prefix  */
    if(strncmp(ftp_url_str, "ftp://", 6))
        return ERR_PARSE_ERROR;
    
    strncpy(ftp_url->url, ftp_url_str,
             NK_MIN(sizeof(ftp_url->url), strlen(ftp_url_str)));
    
    p_substr += 6; /* After ftp:// */
    
    /* Parse username:password */
    if(strstr(p_substr, "@")) {
        /* Get username */
        p_substr1 = strtok(p_substr, ":");
        if(!p_substr1)
            return ERR_PARSE_ERROR;
        strcpy(ftp_url->username, p_substr1);
        p_substr += strlen(p_substr1) + 1;

        /* Get password */
        p_substr1 = strtok(NULL, "@");
        if(!p_substr1)
            return ERR_PARSE_ERROR;
        strcpy(ftp_url->password, p_substr1);
        p_substr += strlen(p_substr1) + 1;
    }
    
    // ftp://ftp.funet.fi/pub/standards/RFC/rfc959.txt
    
    /* Extract resource path.
     * Ignoring the query parameters currently.
     */
    if((ret = NK_parse_fileinfo(strstr(p_substr, "/"), strlen(p_substr),
              &ftp_url->remote_fileinfo)) < 0)
    {
        return ERR_PARSE_ERROR;
    }

    /* Parse host FQDN or IP. */
    p_substr1 = strtok(p_substr, "/");
    if (!p_substr1)
        return ERR_PARSE_ERROR;
    
    /* Check if port is provided. */
    p_substr2 = strstr(p_substr1, ":");
    if(p_substr2) {
        *p_substr2 = '\0';
        strcpy(ftp_url->remote_fqdn, p_substr1);
        p_substr2++;
        ftp_url->remote_port = (p_substr2)
                                ? strtoint(p_substr2, 10)
                                : NK_FTP_DEFAULT_PORT;
    }
    else {
        strcpy(ftp_url->remote_fqdn, p_substr1);
        ftp_url->remote_port = NK_FTP_DEFAULT_PORT;
    }
    
    ret = NK_inet_get_host_ipv4(ftp_url->remote_fqdn,
            ftp_url->remote_ip, sizeof(ftp_url->remote_ip));

    return ret;
}

/**
 * @brief Parse the response string into ftp_conn->current_reponse.
 * 
 * @param ftp_conn 
 * @return int 
 */
static int NK_ftp_parse_response(NK_ftp_connection_t *ftp_conn)
{
    char *recv_data;
    int ret;

    if(!ftp_conn || !ftp_conn->ctl_conn
        || IS_STR_NONE(ftp_conn->ctl_conn->recv_buff)
        || (ftp_conn->ctl_conn->recv_data_len <= 0) )
    {
        return ERR_INVALID_PARAM;
    }
    
    recv_data = ftp_conn->ctl_conn->recv_buff;

    ret = sscanf(recv_data, "%hd %[^\n]s", &ftp_conn->current_response.code,
                        ftp_conn->current_response.message);
    if(ret <= 0)
        return ERR_PARSE_ERROR;
    
    return 0;
}

int NK_ftp_parse_pasv(NK_ftp_connection_t *ftp_conn)
{
    int ret;
    NK_string_list_t addr_parts;
    int16_t port_high, port_low;
    char response_msg[MAX_LEN_RESPONSE_MESSAGE], *open_parenthesis,
         *close_parenthesis;

    if(!ftp_conn || !ftp_conn->ctl_conn
        || (ftp_conn->current_response.code != FTP_RESP_ENTER_PASV)
        || IS_STR_NONE(ftp_conn->current_response.message) )
    {
        return ERR_INVALID_PARAM;
    }

    strncpy(response_msg, ftp_conn->current_response.message,
        NK_MIN(
            strlen(ftp_conn->current_response.message), sizeof(response_msg) - 1
        )
    );

    // response_msg = strtrim(&response_msg, strlen(response_msg));

    open_parenthesis = strstr(response_msg, "(");
    close_parenthesis = strstr(response_msg, ")");
    if(!open_parenthesis || !close_parenthesis)
        return ERR_PARSE_ERROR;
    open_parenthesis++;
    *close_parenthesis = '\0';
    // Now open_parenthesis points to sub-string xx,xx,xx,xx,xx,xx

    if((ret = strsplit(open_parenthesis, strlen(open_parenthesis),
              ",", &addr_parts)) <= 0)
    {
        return ERR_PARSE_ERROR;
    }
    if(ret != 6)
        return ERR_PARSE_ERROR;

    /* IP address part is not really necessary,
     * atleast for now. */
#if 0    
    /* Extract IP component */
    strcpy(ip_addr_str, addr_parts.data[0]);
    strcat(ip_addr_str, ".");
    strcat(ip_addr_str, addr_parts.data[1]);
    strcat(ip_addr_str, ".");
    strcat(ip_addr_str, addr_parts.data[2]);
    strcat(ip_addr_str, ".");
    strcat(ip_addr_str, addr_parts.data[3]);
#endif

    /* Extract port component */
    port_high = strtoint16(addr_parts.data[4], 10); // high 8-bits
    port_low = strtoint16(addr_parts.data[5], 10); // low 8-bits
    ftp_conn->server_data_port = 256 * (int)port_high + (int)port_low;

    return 0;
}

int NK_ftp_make_connection(NK_ftp_connection_t *ftp_conn,
                            const char *remote_ip, int16_t remote_port,
                            const char *user_name, const char *password,
                            char *banner_msg, size_t banner_msg_len)
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

    /* Make a TCP connection used for FTP commands. */

    ftp_conn->ctl_conn = calloc(1, sizeof(NK_tcp_connection_t));
    assert_msg((ftp_conn->ctl_conn != NULL), "ERROR: %s\n", strerror(errno));

    ret = NK_tcp_make_connection(
            ftp_conn->ctl_conn, remote_ip, remote_port, NULL);
    if(ret != 0)
        return ret;
    
    /* Optional: Save the banner message first */
    if(banner_msg && (banner_msg_len > 0)) {
        ret = NK_tcp_recv_until(ftp_conn->ctl_conn, banner_msg,
                banner_msg_len - 1, "\r\n");
        NK_tcp_reset_recv_buff(ftp_conn->ctl_conn);
    }

    response = &ftp_conn->current_response;

    /* Login */

    sprintf(ftp_data_buff, "USER %s\r\n", user_name);
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        if(ret == ERR_REMOTE_DEAD)
            NK_tcp_destroy_connection(ftp_conn->ctl_conn);
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_USERNAME_OK)
        return ERR_AUTH_ERROR;
    
    sprintf(ftp_data_buff, "PASS %s\r\n", password);
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        if(ret == ERR_REMOTE_DEAD)
            NK_tcp_destroy_connection(ftp_conn->ctl_conn);
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_ACCESS_GRANTED)
        return ERR_AUTH_ERROR;

    ftp_conn->is_logged_in = true;
    /* Login successful */

    return 0;
}

int NK_ftp_change_dir(NK_ftp_connection_t *ftp_conn, const char *dir)
{
    int ret;
    NK_ftp_response_t *response;

    if( (!ftp_conn)
        || IS_STR_NONE(dir) )
    {
        return ERR_INVALID_PARAM;
    }

    response = &ftp_conn->current_response;

    sprintf(ftp_data_buff, "CWD %s\r\n", dir);
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_FILE_ACTION_OK)
        return ERR_COMMAND_FAILED;
    
    sprintf(ftp_data_buff, "PWD\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_PATHNAME_CREAT_OK)
        return ERR_COMMAND_FAILED;

    return 0;
}

int NK_ftp_get_file(NK_ftp_connection_t *ftp_conn, const char *filename,
                    const char *dir)
{
    int ret;
    NK_ftp_response_t *response;

    if( (!ftp_conn)
        || IS_STR_NONE(filename)
        || IS_STR_NONE(dir) )
    {
        return ERR_INVALID_PARAM;
    }

    response = &ftp_conn->current_response;

    if((ret = NK_ftp_change_dir(ftp_conn, dir)) < 0)
        return ret;
    
    /* We will consider Image/Binary type data for all data. */
    sprintf(ftp_data_buff, "TYPE I\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_CMD_OK)
        return ERR_COMMAND_FAILED;

    sprintf(ftp_data_buff, "PASV\r\n");
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_ENTER_PASV)
        return ERR_COMMAND_FAILED;
    
    /*  Save PASV data port received from server.   */
    if((ret = NK_ftp_parse_pasv(ftp_conn)) < 0)
        return ret;

    /* Make data connection and download the file. */

    ftp_conn->data_conn = (NK_tcp_connection_t*)malloc(sizeof(NK_tcp_connection_t));
    assert_msg((ftp_conn->data_conn != NULL), "ERROR: %s\n", strerror(errno));
    
    ret = NK_tcp_make_connection(
            ftp_conn->data_conn, ftp_conn->ctl_conn->remote_ip,
            ftp_conn->server_data_port, NULL);
    if(ret < 0)
        return ret;

    /* Send RETR command to initiate file download. */

    sprintf(ftp_data_buff, "RETR %s\r\n", filename);
    if((ret = NK_tcp_sendrecv(ftp_conn->ctl_conn, ftp_data_buff, strlen(ftp_data_buff)))
                    < 0)
    {
        return ret;
    }
    if((ret = NK_ftp_parse_response(ftp_conn)) < 0)
        return ret;
    if(response->code != FTP_RESP_FILE_OK_OPEN_DATACON)
        return ERR_COMMAND_FAILED;

    ret = NK_tcp_recvfile(ftp_conn->data_conn, filename);
    NK_tcp_destroy_connection(ftp_conn->data_conn);
    if(ret < 0)
        return ret;
    
    return 0;
}

/* TODO: FTP receive callback. */

