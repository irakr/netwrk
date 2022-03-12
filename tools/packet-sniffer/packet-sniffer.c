#include <stdio.h>
#include "netwrk/netwrk.h"

#define USAGE_STR	\
	"packet-sniffer <packet-count>\n"

int main(int argc, const char *argv[])
{
	int ret;

	/*
	 * Parse command-line arguments.
	 */

	if(argc != 2) {
		fprintf(stderr, "ERROR: Arguments not provided.\n");
		printf("\nUsage:\n%s", USAGE_STR);
		return EXIT_FAILURE;
	}
	
	printf("################################################\n"
		"\tPacket sniffer tool - by irakr\n"
		"################################################\n");
	
	return 0;
}

