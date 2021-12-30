#pragma once

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
} NK_ftp_command_t;

/*
 * FTP request data
 */
typedef struct _NK_ftp_request_t {
    enum{
        GET = 1,
        PUT = 2
    } request_type;
    
} NK_ftp_request_t;

/*
 * FTP response data
 */
typedef struct _NK_ftp_response_t {

} NK_ftp_response_t;

/*
 * FTP context data
 */
typedef struct _NK_ftp_ctx_t {
    NK_ftp_user_info_t* current_user;
    NK_ftp_request_t* current_request;
    NK_ftp_response_t* current_response;
} NK_ftp_ctx_t;

