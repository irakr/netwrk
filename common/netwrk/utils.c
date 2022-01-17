#include "netwrk/netwrk.h"
#include "netwrk/utils.h"
#include <stdio.h>
#include <string.h>

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
