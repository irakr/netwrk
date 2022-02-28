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
 * @brief Check if string is NULL pointer or if not
 *   then is it an empty string.
 * 
 */
#define IS_STR_NONE(str)    (!str || !strlen(str))

/**
 * @brief String list
 * 
 */
typedef struct
{
    char **data;
    int rows;
} NK_string_list_t;

int16_t strtoint16(const char *s, int base);
int strtoint(const char *s, int base);

/**
 * @brief Returns the number of occurences of the sub-string @sub_str
 *   in @s.
 * 
 * @param s 
 * @param sub_str
 * @param len 
 * @return int Returns the count.
 */
int stroccurence(const char *s, size_t len, const char *sub_str);

/**
 * @brief Left and right trim a string @s.
 * 
 * @param s 	Pointer to the pointer that points to the actual string to be trimmed.
 * @param len 	Length of the string buffer provided by caller.
 * @return char* Pointer to the trimmed string. If @s contains only whitespaces then
 *   a string with first character as '\0' is returned. Otherwise NULL is returned for
 *   all other errors.
 */
char* strtrim(char **s, size_t len);

/**
 * @brief Split string using @delim delimiter and store each token in @result.
 * 
 * @param s 
 * @param len 
 * @param delim Set of delimeters as a string. Passed to strtok().
 * @param result
 * @return int Returns the number of items added to the list @result.
 */
int strsplit(char *s, size_t len, const char *delim, NK_string_list_t *result);

/**
 * @brief Wrapper over memmem(). It just corrects the address value
 *   returned by memmem().
 * 
 * @param haystack 
 * @param haystacklen 
 * @param needle 
 * @param needlelen 
 * @return void* 
 */
void *memmem_c(const void *haystack, size_t haystacklen,
                    const void *needle, size_t needlelen);

/**
 * @brief Read string input from stdin. Repeats reading if input is empty
 * or contains only whitespaces.
 * 
 * @param in 
 * @param max_len 
 * @return int 
 */
int shell_input(char *in, ssize_t max_len);

/**
 * @brief A simple data structure to store file path components.
 * 
 */
typedef struct _NK_file_info_t
{
	char target_filepath[1024]; /* Original file path */
	char directory[1024];  /* Directory portion */
	char *filename; /* Filename portion */
} NK_file_info_t;

/**
 * @brief Parse file path.
 * 
 * @param filepath 
 * @param len 
 * @param file_info 
 * @return int 
 */
int NK_parse_fileinfo(const char *filepath, size_t len, NK_file_info_t *file_info);

