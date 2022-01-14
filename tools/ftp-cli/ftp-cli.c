#include <stdio.h>
#include <stdlib.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

// Test server
#define FTP_SERVER_IP	"35.163.228.146"
#define FTP_SERVER_PORT	21

#define MAX_CMD_LEN		2049

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
