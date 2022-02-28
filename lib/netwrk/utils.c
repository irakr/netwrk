#include <stdio.h>
#include "netwrk/netwrk.h"
#include "netwrk/utils.h"

int16_t strtoint16(const char *s, int base)
{
	return (int16_t)strtoint(s, base);
}

int strtoint(const char *s, int base)
{
	long int ret;
	ret = strtol(s, NULL, base);
	if( !ret || (ret == INT64_MIN) || (ret == INT64_MAX) )
		return -1;
	return (int)ret;
}

int stroccurence(const char *s, size_t len, const char *sub_str)
{
	const char *s_ptr;
	int count = 0;

	if(!s || !sub_str)
		return ERR_INVALID_PARAM;
	
	for(s_ptr = s; (s_ptr = strstr(s_ptr, sub_str)); ++s_ptr, ++count);
	
	return count;
}

char* strtrim(char **s, size_t len)
{
    int i;

	if(!s || !(*s))
		return NULL;
	
	// Left trim
	for(i = 0; (i < len) && isspace((*s)[i]); ++i);
	if( (i >= len) ) {
		**s = '\0';
		return *s;
	}
	*s += i;
	
	// Right trim
    for (i = strlen(*s) - 1; (i >= 0) && isspace((*s)[i]); i--);
    (*s)[i + 1] = '\0';
    return *s;
}

int strsplit(char *s, size_t len, const char *delim, NK_string_list_t *result)
{
	int i, token_count;
	char *token;

	if(IS_STR_NONE(s) || !result || IS_STR_NONE(delim))
		return ERR_INVALID_PARAM;
	
	result->rows = 0; // Reset

	// Allocate memory for rows.
	token_count = stroccurence(s, len, delim);
	if(token_count <= 0)
		return 0;
	result->data = (char**)calloc(token_count, sizeof(char*));
	if(!result->data)
		return ERR_MEMORY_ERROR;
	
	token = strtok(s, delim);
	for(i = 0; token; ++i) {
		result->data[i] = calloc(strlen(token) + 1, sizeof(char));
		
		// Return whatever has been successful so far.
		if(!result->data[i])
			return result->rows;
		
		strcpy(result->data[i], token);
		result->rows++;
		token = strtok(NULL, delim);
	}

	return result->rows;
}

void *memmem_c(const void *haystack, size_t haystacklen,
                    const void *needle, size_t needlelen)
{
	return (void*)(memmem(haystack, haystacklen, needle, needlelen)
			& 0x7fffffffffff);
}

int shell_input(char *in, ssize_t max_len)
{
	int len = 0;
	if( !in || (max_len <= 0) )
		return -1;

	do {
		fflush(stdin);
		*in = '\0'; // For safety.
		fgets(in, max_len, stdin);
		strtrim(&in, strlen(in));
	} while(!(len = strlen(in)));
	return len;
}

int NK_parse_fileinfo(const char *filepath, size_t len, NK_file_info_t *file_info)
{
	if( IS_STR_NONE(filepath)
		|| (len <= 0) || !file_info )
	{
		return ERR_INVALID_PARAM;
	}
	
	strcpy(file_info->target_filepath, filepath);
	file_info->filename = strrchr(filepath, '/') + 1;
	strncpy(file_info->directory, filepath, file_info->filename - filepath);

	return 0;
}

