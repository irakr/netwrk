/**
 * @file packet-sniffer.c
 * @author Irak Rigia
 * @brief A simple packet sniffing tool.
 * @version 0.1
 * @date 2022-03-12
 */
#include <stdio.h>
#include "netwrk/netwrk.h"
#include "netwrk/netif_utils.h"
#include <unistd.h>

#define USAGE_STR	\
	"packet-sniffer <packet-count> <capture-file>\n"

#define DEFAULT_PACKET_COUNT	10

void print_ethhdr_info(const unsigned char *packet)
{
	struct ethhdr *eth_hdr;
	if(!packet) {
		assert_msg(packet, "ERROR: %s(): Invalid argument for packet.\n", __FUNCTION__);
		return;
	}

	eth_hdr = (struct ethhdr *)packet;
	printf("[eth] proto: 0x%X(%s), "
			"src : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X, "
			"dst : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
			ntohs(eth_hdr->h_proto), (ntohs(eth_hdr->h_proto) == 0x800) ? "IP" : "X",
			eth_hdr->h_source[0], eth_hdr->h_source[1], eth_hdr->h_source[2], eth_hdr->h_source[3], eth_hdr->h_source[4], eth_hdr->h_source[5],
			eth_hdr->h_dest[0], eth_hdr->h_dest[1], eth_hdr->h_dest[2], eth_hdr->h_dest[3], eth_hdr->h_dest[4], eth_hdr->h_dest[5]);
}

void print_iphdr_info(const unsigned char *packet)
{
	struct iphdr *ip_hdr;
	struct ethhdr *eth_hdr;
	if(!packet) {
		assert_msg(packet, "ERROR: %s(): Invalid argument for packet.\n", __FUNCTION__);
		return;
	}

	eth_hdr = (struct ethhdr *)packet;
	if(ntohs(eth_hdr->h_proto) != 0x800) {
		fprintf(stderr, "ERROR: Packet is not an IP packet.\n");
		return ERR_NET_INVALID_PROTOCOL;
	}
	ip_hdr = (struct iphdr*)(packet + sizeof(struct ethhdr));
	
	// TODO
}

int main(int argc, const char *argv[])
{
	int ret;
	int sock_raw;
	// int packet_count = DEFAULT_PACKET_COUNT;
	unsigned char *recv_buff;
	struct sockaddr saddr;
	struct ethhdr *eth_hdr;
	int saddr_len = sizeof(struct sockaddr);
	ssize_t buflen;
	char if_name[IFNAMSIZ];

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
	
	/* Get interface name from command line arg. */
	if(IS_STR_NONE(argv[1])) {
		fprintf(stderr, "ERROR: Invalid interface name.\n");
		return EXIT_FAILURE;
	}
	strncpy(if_name, argv[1], IFNAMSIZ - 1);

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
		fprintf(stderr, "ERROR: Failed to bind socket fd %d to the interface %s.\n",
				sock_raw, if_name);
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
		
		/* Print ETH header info	*/
		print_ethhdr_info(recv_buff);

		/* Print IP header info */
		print_iphdr_info(recv_buff);

		usleep(1000000);
	}
	
	return 0;
}

