#include <stdio.h>
#include "netwrk/netwrk.h"
#include "netwrk/netif_utils.h"
#include <unistd.h>

#define USAGE_STR	\
	"packet-sniffer <packet-count> <capture-file>\n"

#define DEFAULT_PACKET_COUNT	10

void print_eth_info(struct ethhdr *eth_hdr)
{
	if(!eth_hdr) {
		assert_msg(eth_hdr, "ERROR: %s(): Invalid argument eth_hdr.\n", __FUNCTION__);
		return;
	}
	printf("\nEthernet Header\n");
	printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth_hdr->h_source[0], eth_hdr->h_source[1], eth_hdr->h_source[2], eth_hdr->h_source[3], eth_hdr->h_source[4], eth_hdr->h_source[5]);
	printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth_hdr->h_dest[0], eth_hdr->h_dest[1], eth_hdr->h_dest[2], eth_hdr->h_dest[3], eth_hdr->h_dest[4], eth_hdr->h_dest[5]);
	printf("\t|-Protocol : 0x%X\n", ntohs(eth_hdr->h_proto));
}

int test_main()
{
	int ret;
	int sock_raw;
	unsigned char *recv_buff;
	struct sockaddr saddr;
	struct ethhdr *eth_hdr;
	int saddr_len = sizeof(struct sockaddr);
	ssize_t buflen;
	const char *if_name = "eth0";

	/* Create a raw socket. */
	sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));/* See man packet(7) for the details. */
	if (sock_raw < 0) {
		fprintf(stderr, "ERROR: %s\n", strerror(errno));
		return -1;
	}

	/* Bind the interface @if_name to the raw socket.	*/
	printf("Binding to the interface %s....", if_name); fflush(stdout);
	ret = netif_bind_sock_to_ifname(sock_raw, if_name);
	if(ret < 0) {
		printf("FAILED\n"); fflush(stdout);
		fprintf(stderr, "ERROR: Failed to bind socket fd %d to the interface %s.\n", sock_raw, if_name);
		return -1;
	}
	printf("DONE\n"); fflush(stdout);
	printf("Interface index of %s: %d\n", if_name, netif_get_ifr_index(if_name));

	recv_buff = (unsigned char*)malloc(65536);
	assert_msg(recv_buff, "ERROR: malloc() failed: %s.\n", strerror(errno));
	memset(recv_buff, 0, 65536);

	while(1) {
		
		/* Receive a network packet and copy in to buffer.	*/
		buflen = recvfrom(sock_raw, recv_buff, 65536, 0, &saddr, (socklen_t *)&saddr_len);
		if (buflen < 0) {
			fprintf(stderr, "ERROR: recvfrom() failed: %s\n", strerror(errno));
			return -1;
		}

		eth_hdr = (struct ethhdr *)recv_buff;
		print_eth_info(eth_hdr);

		usleep(1000000);
	}

	

	return 0;
}

int main(int argc, const char *argv[])
{
	int ret;
	int packet_count = DEFAULT_PACKET_COUNT;
	
	exit(test_main());

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

