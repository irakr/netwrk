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

/**
 * @brief Error codes
 * 
 */
#define ERR_OK                   0
#define ERR_UNKNOWN             -1
#define ERR_INVALID_PARAM       -2
#define ERR_INVALID_SOCKET      -3
#define ERR_INVALID_ADDRESS     -4
#define ERR_CONNECTION_ERROR    -5
#define ERR_PTHREAD_ERROR       -6
#define ERR_NOT_IMPLEMENTED     -7
#define ERR_WRITE_ERROR         -8
#define ERR_READ_ERROR          -9
#define ERR_REMOTE_DEAD         -10
#define ERR_INVALID_CONNECTION  -11

