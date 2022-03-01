#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include "netwrk/inet.h"

int NK_inet_get_host_ipv4(const char *host_fqdn, char *ip_str, size_t ip_len)
{
    struct addrinfo hints, *res, *p_res;
    struct sockaddr_in *ipv4_addr;
    void *addr;
    int ret;

    if(IS_STR_NONE(host_fqdn) || !ip_str
       || (ip_len < NK_MAX_IPV4_LEN))
        return ERR_INVALID_PARAM;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((ret = getaddrinfo(host_fqdn, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "%s(): getaddrinfo: %s\n",
                __FUNCTION__, gai_strerror(ret));
        return ERR_INVALID_PARAM;
    }
    for(p_res = res; p_res; p_res = p_res->ai_next) {
        ipv4_addr = (struct sockaddr_in *)p_res->ai_addr;
        addr = &(ipv4_addr->sin_addr);
        inet_ntop(p_res->ai_family, addr, ip_str, ip_len);
    }
    freeaddrinfo(res);

    return 0;
}