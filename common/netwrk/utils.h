#pragma once

#include <sys/types.h>
#include <ctype.h>

/**
 * @brief Max and Min utility functions.
 * 
 */
#define NK_MAX(a, b)                \
    (                               \
        {                           \
            __typeof__(a) _a = (a); \
            __typeof__(b) _b = (b); \
            _a > _b ? _a : _b;      \
        })
#define NK_MIN(a, b)                \
    (                               \
        {                           \
            __typeof__(a) _a = (a); \
            __typeof__(b) _b = (b); \
            _a <= _b ? _a : _b;      \
        })

/**
 * @brief Left and right trim a string @s.
 * 
 * @param s 	Pointer to the pointer that points to the actual string to be trimmed.
 * @param len 	Length of the string buffer provided by caller.
 * @return char* Pointer to the trimmed string. If @s contains only whitespaces then
 * a string with first character as '\0' is returned.
 */
char* strtrim(char **s, size_t len);

/**
 * @brief Read string input from stdin. Repeats reading if input is empty
 * or contains only whitespaces.
 * 
 * @param in 
 * @param max_len 
 * @return int 
 */
int shell_input(char *in, ssize_t max_len);

