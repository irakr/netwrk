#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

// Test server
#define FTP_SERVER_IP	"35.163.228.146"

int main()
{
	NK_ftp_user_info_t user_info;
	int tcp_sock_fd, conn_sock_fd;

	printf("################ Simple FTP client ################\n");
	
	printf("Username: ");
	scanf("%s", user_info.username);
	printf("Password: ");
	scanf("%s", user_info.password);
	
	tcp_sock_fd = NK_tcp_create_socket();
	if(tcp_sock_fd < 0)
		return EXIT_FAILURE;

	conn_sock_fd = NK_tcp_connect_server(tcp_sock_fd, FTP_SERVER_IP, 21);
	if(conn_sock_fd < 0)
		return EXIT_FAILURE;
	
	printf("Connection successful to %s\n", FTP_SERVER_IP);

	NK_tcp_disconnect_server(conn_sock_fd);
	close(tcp_sock_fd);
	
	return 0;
}
