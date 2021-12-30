#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

// Test server
#define FTP_SERVER_IP	"35.163.228.146"
static char recv_buff[NK_MAX_TCP_PKT_SIZE];

int main()
{
	NK_ftp_user_info_t user_info;
	int tcp_sock_fd;
	int disconnect = 0;

	printf("################ Simple FTP client ################\n");
	
	printf("Username: ");
	scanf("%s", user_info.username);
	printf("Password: ");
	scanf("%s", user_info.password);
	
	tcp_sock_fd = NK_tcp_create_socket();
	if(tcp_sock_fd < 0)
		return EXIT_FAILURE;

	if(NK_tcp_connect(tcp_sock_fd, FTP_SERVER_IP, 21) < 0)
		return EXIT_FAILURE;
	
	printf("++++++++++ Connected to %s ++++++++++\n", FTP_SERVER_IP);

	// Test loop
	while (!disconnect) {
		recv(tcp_sock_fd, recv_buff, NK_MAX_TCP_PKT_SIZE, 0);
		printf("%s", recv_buff);
		disconnect = 1;
	}

	NK_tcp_disconnect(tcp_sock_fd);

	return 0;
}
