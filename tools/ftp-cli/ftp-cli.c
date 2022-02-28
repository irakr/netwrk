#include <stdio.h>
#include "netwrk/netwrk.h"
#include "netwrk/tcp_socket.h"
#include "netwrk/ftp.h"

#define USAGE_STR	\
	"ftp-cli <remote-filepath>\n"

// Test server data
// TODO: These are to be provided by user with cmd args.
#define FTP_SERVER_IP		"134.119.8.75"
#define FTP_SERVER_PORT					21
#define FTP_TARGET_FILE		""

#define MAX_CMD_LEN		2049

char banner_msg[512];

int main(int argc, const char *argv[])
{
	NK_ftp_connection_t ftp_conn;
	NK_file_info_t file_info;

	/*
	 * Parse command-line arguments.
	 */

	if(argc == 1) {
		fprintf(stderr, "ERROR: Please provide the path of the file "
				"to be downloaded.\n");
		printf("\nUsage:\n%s", USAGE_STR);
		return EXIT_FAILURE;
	}
	
	if(NK_parse_fileinfo(argv[1], strlen(argv[1]), &file_info) < 0) {
		fprintf(stderr, "ERROR: Failed to parse input filename.\n");
		return EXIT_FAILURE;
	}

	printf("################################################\n"
		"\tSimple FTP file downloader - by irakr\n"
		"################################################\n");
	
	/*
	 * Establish connection and LOGIN.
	 */
	if(NK_ftp_make_connection(&ftp_conn,
			FTP_SERVER_IP, FTP_SERVER_PORT, NULL, NULL,
			banner_msg, sizeof(banner_msg))
		< 0)
	{
		return EXIT_FAILURE;
	}
	
	printf("[Connected]\n"
		   "IP: %s\nPort:%d\n",
			FTP_SERVER_IP, FTP_SERVER_PORT);
	printf("+++++++++++++ Banner message from the server +++++++++++++\n%s"
		   "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n",
		   banner_msg);

	if(NK_ftp_get_file(&ftp_conn, file_info.filename, file_info.directory) < 0) {
		fprintf(stderr, "ERROR: Failed to download file /zoc/zoc.exe\n");
		return EXIT_FAILURE;
	}
	printf("\nDownload completed.\n");
	
	/* End connection. */

	printf("Closing connection...");
	NK_tcp_destroy_connection(ftp_conn.tcp_conn);
	printf("DONE\n");

	return 0;
}

