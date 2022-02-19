#pragma once

#include "netwrk/netwrk.h"
#include "netwrk/tcp_socket.h"

#define MAX_LEN_USERNAME	128
#define MAX_LEN_PASSWORD	256

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
    const char *message;
    void *data;
} NK_ftp_response_t;

/*
 * FTP connection information
 */
typedef struct _NK_ftp_connection_t {
    NK_tcp_connection_t *tcp_conn;

#define LOGIN_MODE_ANONYMOUS    1
#define LOGIN_MODE_USER         2
    int login_mode;
    bool is_logged_in;
    NK_ftp_user_info_t current_user;

    NK_ftp_request_t *current_request;
    NK_ftp_response_t *current_response;
} NK_ftp_connection_t;

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
 * @return int 
 */
int NK_ftp_make_connection(NK_ftp_connection_t *ftp_conn,
                            const char *remote_ip, int16_t remote_port,
                            const char *user_name, const char *password);

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

