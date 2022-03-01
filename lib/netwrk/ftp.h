#pragma once

#include "netwrk/netwrk.h"
#include "netwrk/tcp_socket.h"

#define NK_FTP_DEFAULT_PORT         21

#define MAX_LEN_USERNAME	        128
#define MAX_LEN_PASSWORD	        256
#define MAX_LEN_RESPONSE_MESSAGE    1024

/*
 * FTP user credentials
 */
typedef struct _NK_ftp_user_info_t {
    char username[MAX_LEN_USERNAME];
    char password[MAX_LEN_PASSWORD];
} NK_ftp_user_info_t;

typedef enum {
    USER = 1
    , PASS          // 2
    , LIST          // 3
    , CWD           // 4
    , PORT          // 5
    , PASV          // 6
} NK_ftp_command_t;

typedef enum {
FTP_RESP_CMD_OK                             = 200,
FTP_RESP_CMD_SYNTAX_ERROR                   = 500,
FTP_RESP_ARG_SYNTAX_ERROR                   = 501,
FTP_RESP_CMD_NOT_IMPL                       = 502,
FTP_RESP_CMD_NOT_IMPL_SUP                   = 202, /* Superflous at this site */
FTP_RESP_CMD_BAD_SEQ                        = 503,
FTP_RESP_CMD_NOT_IMPL_PAR                   = 504,

FTP_RESP_USERNAME_OK                        = 331,
FTP_RESP_ACCESS_GRANTED                     = 230,
FTP_RESP_FILE_ACTION_OK                     = 250,
FTP_RESP_PATHNAME_CREAT_OK                  = 257,
FTP_RESP_ENTER_PASV                         = 227,
FTP_RESP_FILE_TRANSFER_COMPLETE             = 226,
FTP_RESP_FILE_OK_OPEN_DATACON               = 150
} NK_ftp_response_code_t;

/*
 * FTP request data
 */
typedef struct _NK_ftp_request_t {
    NK_ftp_command_t command;
    char *argument;
} NK_ftp_request_t;

/*
 * FTP response data
 */
typedef struct _NK_ftp_response_t {
    int16_t code;
    char message[MAX_LEN_RESPONSE_MESSAGE];
    void *data;
} NK_ftp_response_t;

/*
 * FTP connection information
 */
typedef struct _NK_ftp_connection_t {
    NK_tcp_connection_t *tcp_conn, *data_conn;

#define LOGIN_MODE_ANONYMOUS    1
#define LOGIN_MODE_USER         2
    int login_mode;
    bool is_logged_in;
    NK_ftp_user_info_t current_user;

    NK_ftp_request_t current_request;  /* Not used currently */
    NK_ftp_response_t current_response;

    int server_data_port;   /* Data port received from server PASV response */

} NK_ftp_connection_t;

/**
 * @brief Data structure that defines an FTP URL.
 * 
 */
#define NK_FTP_MAX_URL_LEN          1024
#define NK_FTP_MAX_FQDN_LEN         256
#define NK_FTP_MAX_USERNAME_LEN     128
#define NK_FTP_MAX_PASSWORD_LEN     128
typedef struct _NK_ftp_url_t
{
    /* URL string. */
    char url[NK_FTP_MAX_URL_LEN];

    /* Credentials */
    char username[NK_FTP_MAX_USERNAME_LEN],
         password[NK_FTP_MAX_PASSWORD_LEN];
    
    /* Remote host address  */
    char remote_fqdn[NK_FTP_MAX_FQDN_LEN], remote_ip[NK_MAX_IPV4_LEN];
    int remote_port;

    /* Remote file path info    */
    NK_file_info_t remote_fileinfo;

} NK_ftp_url_t;

/**
 * @brief Parse an FTP URL provided in @ftp_url into @furl.
 * 
 * @param ftp_url_str Original FTP URL string.
 * @param ftp_url Parsed FTP info.
 * @return int 
 */
int NK_ftp_parse_url(const char *ftp_url_str, NK_ftp_url_t *ftp_url);

/**
 * @brief Login to FTP server at @remote_ip:@remote_port using the
 *   credentials @user_name and @password. This function will login
 *   anonymously if @user_name is empty string or NULL.
 * 
 * @param ftp_conn 
 * @param remote_ip 
 * @param remote_port 
 * @param user_name If empty string or NULL then anonymous login will be
 *   performed.
 * @param password 
 * @param banner_msg (Optional) You can provide a string buffer to get the
 *   banner message string sent by the server.
 * @param banner_msg_len Length of banner_msg buffer.
 * @return int 
 */
int NK_ftp_make_connection(NK_ftp_connection_t *ftp_conn,
                            const char *remote_ip, int16_t remote_port,
                            const char *user_name, const char *password,
                            char *banner_msg, size_t banner_msg_len);

/**
 * @brief Go to the directory @dir using FTP CWD command.
 * 
 * @param ftp_conn 
 * @param dir Target directory.
 * @return int 
 */
int NK_ftp_change_dir(NK_ftp_connection_t *ftp_conn, const char *dir);

/**
 * @brief Download the file @filename located in the directory @dir.
 * 
 * @param ftp_conn 
 * @param filename 
 * @param dir 
 * @return int 
 */
int NK_ftp_get_file(NK_ftp_connection_t *ftp_conn, const char *filename,
                    const char *dir);

/**
 * @brief Parse the server port from the PASV response and store in
 *   ftp_conn->server_data_port. 
 * 
 * @param ftp_conn 
 * @return int 
 */
int NK_ftp_parse_pasv(NK_ftp_connection_t *ftp_conn);

