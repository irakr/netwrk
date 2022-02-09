#include <stdio.h>
#include <stdlib.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

// Test server
#define FTP_SERVER_IP	"134.119.8.75"
#define FTP_SERVER_PORT	21

#define MAX_CMD_LEN		2049

int main()
{
	NK_ftp_connection_t ftp_conn;
	// NK_ftp_user_info_t user_info;
	char input_cmd[MAX_CMD_LEN];
	int cmd_len = 0, exit = 0;

	printf("################ Simple FTP client - by irakr ################\n");
	
	if(NK_ftp_make_connection(&ftp_conn, FTP_SERVER_IP, FTP_SERVER_PORT, NULL, NULL) < 0)
		return EXIT_FAILURE;
	
	printf("++++++++++ Connected to FTP server: %s:%d ++++++++++\n", FTP_SERVER_IP, FTP_SERVER_PORT);

	while(!exit) {
		if( (cmd_len = shell_input(input_cmd, MAX_CMD_LEN)) < 0) {
			exit = 1;
			break;
		}
		if(send(ftp_conn.tcp_conn->sock_fd, input_cmd, cmd_len, 0) <= 0)
			exit = 1;
	}

	// End connection.
	NK_tcp_destroy_connection(ftp_conn.tcp_conn);

	return 0;
}
