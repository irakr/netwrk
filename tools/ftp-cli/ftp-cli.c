#include <stdio.h>
#include "netwrk/netwrk.h"
#include "netwrk/tcp_conn.h"
#include "netwrk/ftp.h"

#define USAGE_STR	\
	"ftp-cli <ftp-url>\n"

char banner_msg[512];

int main(int argc, const char *argv[])
{
	NK_ftp_connection_t ftp_conn;
	NK_ftp_url_t ftp_url;
	int ret;

	/*
	 * Parse command-line arguments.
	 */

	if(argc != 2) {
		fprintf(stderr, "ERROR: Please provide the path of the file "
				"to be downloaded.\n");
		printf("\nUsage:\n%s", USAGE_STR);
		return EXIT_FAILURE;
	}
	
	printf("################################################\n"
		"\tSimple FTP file downloader - by irakr\n"
		"################################################\n");
	

	if((ret = NK_ftp_parse_url(argv[1], &ftp_url)) < 0) {
		fprintf(stderr, "ERROR(%d): Failed to parse URL.\n", ret);
		return EXIT_FAILURE;
	}

	/*
	 * Establish connection and LOGIN.
	 */
	if((ret = NK_ftp_make_connection(&ftp_conn,
			ftp_url.remote_ip, ftp_url.remote_port, ftp_url.username,
			ftp_url.password, banner_msg, sizeof(banner_msg)))
		< 0)
	{
		fprintf(stderr, "ERROR(%d): Failed to connect to the server.\n", ret);
		return EXIT_FAILURE;
	}
	
	printf("[Connected]\n"
		   "IP: %s\nPort:%d\n",
			ftp_url.remote_ip, ftp_url.remote_port);
	printf("+++++++++++++ Banner message from the server +++++++++++++\n%s"
		   "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n",
		   banner_msg);

	if((ret = NK_ftp_get_file(&ftp_conn, ftp_url.remote_fileinfo.filename,
			ftp_url.remote_fileinfo.directory)) < 0)
	{
		fprintf(stderr, "ERROR(%d): Failed to download file %s.\n",
				ret, ftp_url.remote_fileinfo.filename);
		return EXIT_FAILURE;
	}
	printf("\nDownload completed.\n");
	
	/* End connection. */

	printf("Closing connection...");
	NK_tcp_destroy_connection(ftp_conn.tcp_conn);
	printf("DONE\n");

	return 0;
}

