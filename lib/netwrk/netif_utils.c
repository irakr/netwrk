#include "netwrk/netif_utils.h"

int netif_get_ifr_index(const char *if_name)
{
	int sock_raw;
	struct ifreq ifreq_i;

    if(IS_STR_NONE(if_name))
        return ERR_INVALID_PARAM;
    
	sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
	if(sock_raw < 0) {
		fprintf(stderr, "ERROR: %s\n", strerror(errno));
		return ERR_NET_SOCKET_FAILED;
	}
	
	memset(&ifreq_i, 0, sizeof(ifreq_i));
	strncpy(ifreq_i.ifr_name, if_name, IFNAMSIZ - 1); // giving name of Interface

	if((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0) {
		fprintf(stderr, "ERROR: ioctl() failed: %s\n", strerror(errno)); // getting Index Name
		return ERR_SYS_IOCTL_FAILED;
	}

	return ifreq_i.ifr_ifindex;
}

int netif_bind_sock_to_ifname(int sock_fd, const char *if_name)
{
	int ret;
	struct sockaddr_ll addr_ll; /* To bind our raw socket to a particular interface.	*/
	size_t if_name_len;

	if( (sock_fd < 0) || IS_STR_NONE(if_name) )
		return ERR_INVALID_PARAM;

	if_name_len = strnlen(if_name, IFNAMSIZ);
	if(if_name_len >= IFNAMSIZ) {
		fprintf(stderr, "ERROR: Interface name too long.\n");
		return ERR_NET_INVALID_INTF;
	}

#ifdef USE_SETSOCKOPT /* setsockopt() method (FIXME) */
	ret = setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, if_name, if_name_len);
	if(ret < 0) {
		fprintf(stderr, "ERROR: setsockopt(): %s.\n", strerror(errno));
		return ERR_NET_BIND_FAILED;
	}
#else /* struct sockaddr_ll method */
	addr_ll.sll_family = AF_PACKET;
	addr_ll.sll_ifindex = netif_get_ifr_index(if_name);
	addr_ll.sll_protocol = htons(ETH_P_ALL);
	if(addr_ll.sll_ifindex < 0)
		return ERR_NET_INVALID_INTF;
	ret = bind(sock_fd, &addr_ll, sizeof(struct sockaddr_ll));
	if(ret < 0) {
		fprintf(stderr, "ERROR: bind(): %s.\n", strerror(errno));
		return ERR_NET_BIND_FAILED;
	}
#endif
	
}