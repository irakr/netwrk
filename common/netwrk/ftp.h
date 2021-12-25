#pragma once

#define MAX_LEN_USERNAME	128
#define MAX_LEN_PASSWORD	256

typedef struct _ftp_user_info_t {
    char username[MAX_LEN_USERNAME];
    char password[MAX_LEN_PASSWORD];
} ftp_user_info_t;