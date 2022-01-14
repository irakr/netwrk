#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

// Test server
#define FTP_SERVER_IP	"35.163.228.146"
#define FTP_SERVER_PORT	21

#define MAX_CMD_LEN		2049

/**
 * @brief Left and right trim a string @s.
 * 
 * @param s 	Pointer to the pointer that points to the actual string to be trimmed.
 * @param len 	Length of the string buffer provided by caller.
 * @return char* Pointer to the trimmed string. If @s contains only whitespaces then
 * a string with first character as '\0' is returned.
 */
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

/**
 * @brief Read string input from stdin. Repeats reading if input is empty
 * or contains only whitespaces.
 * 
 * @param in 
 * @param max_len 
 * @return int 
 */
int shell_input(char *in, ssize_t max_len)
{
	int len = 0;
	if( !in || (max_len <= 0) ) {
		fprintf(stderr, "ERROR: %s(): Invalid function argument.", __func__);
		return -1;
	}

	do {
		fflush(stdin);
		*in = '\0'; // For safety.
		fgets(in, max_len, stdin);
		strtrim(&in, strlen(in));
	} while(!strlen(in));
	return len;
}

int main()
{
	NK_tcp_connection_t tcp_conn;
	// NK_ftp_user_info_t user_info;
	char input_cmd[MAX_CMD_LEN];
	int cmd_len = 0, exit = 0;

	printf("################ Simple FTP client - by irakr ################\n");
	
	if(NK_tcp_make_connection(&tcp_conn, FTP_SERVER_IP, FTP_SERVER_PORT, NULL) < 0)
		return EXIT_FAILURE;
	
	printf("++++++++++ Connected to %s:%d ++++++++++\n", FTP_SERVER_IP, FTP_SERVER_PORT);

	while(!exit) {
		if( (cmd_len = shell_input(input_cmd, MAX_CMD_LEN)) < 0) {
			exit = 1;
			break;
		}
		if(send(tcp_conn.sock_fd, input_cmd, cmd_len, 0) <= 0)
			exit = 1;
	}

	// End connection.
	NK_tcp_destroy_connection(&tcp_conn);

	return 0;
}
