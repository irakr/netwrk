#pragma once

#include <errno.h>
#include <assert.h>
#include <stdio.h>

/**
 * @brief Custom assert() function that prints useful message
 *   before aborting.
 * 
 */
#ifdef assert_msg
#undef assert_msg
#endif
#define assert_msg(cond, format_str, ...)   \
        assert(cond || (fprintf(stderr, format_str, __VA_ARGS__)) )

/* General error codes */
#define ERR_OK                          0
#define ERR_UNKNOWN                     -1
#define ERR_INVALID_PARAM               -2
#define ERR_INVALID_SOCKET              -3
#define ERR_INVALID_ADDRESS             -4
#define ERR_CONNECTION_ERROR            -5
#define ERR_PTHREAD_ERROR               -6
#define ERR_NOT_IMPLEMENTED             -7
#define ERR_WRITE_ERROR                 -8
#define ERR_READ_ERROR                  -9
#define ERR_READ_TIMEDOUT               -10
#define ERR_REMOTE_DEAD                 -11
#define ERR_INVALID_CONNECTION          -12
#define ERR_MEMORY_ERROR                -13
#define ERR_PARSE_ERROR                 -14
#define ERR_DATA_NOT_FOUND              -15
#define ERR_BUFFER_FULL                 -16
#define ERR_BUFFER_CLEARED              -17
#define ERR_AUTH_ERROR                  -18
#define ERR_COMMAND_FAILED              -19
#define ERR_FILE_OPEN_ERROR             -20
#define ERR_INDEX_OUT_OF_BOUND          -21

/* Network error codes  */
#define ERR_NET_INVALID_INTF            -22
#define ERR_NET_BIND_FAILED             -23
#define ERR_NET_SOCKET_FAILED           -24
#define ERR_NET_INVALID_PROTOCOL        -25

/* Operating system specific error codes    */
#define ERR_SYS_IOCTL_FAILED            -26

