#pragma once

#include "netwrk/netwrk.h"
#include <sys/socket.h>
#include <linux/if_ether.h> /* For ETH_P_ALL	*/
#include <linux/if_packet.h> /* For struct sockaddr_ll	*/
#include <netinet/in.h>	/* For IPPROTO_RAW	*/
#include <sys/ioctl.h>
#include <net/if.h>	/* For struct ifreq	*/
#include <net/ethernet.h>
#include <linux/ip.h>   /* For struct iphdr */

/**
 * @brief Returns the index of the interface @if_name.
 * 
 */
int netif_get_ifr_index(const char *if_name);

/**
 * @brief Bind the socket @sock_fd to the interface named @if_name.
 * 
 * @param sock_fd 
 * @param if_name 
 * @return int 
 */
int netif_bind_sock_to_ifname(int sock_fd, const char *if_name);

