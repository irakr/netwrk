#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include "netwrk/netwrk.h"

#define NK_MAX_IPV4_LEN    INET_ADDRSTRLEN
#define NK_MAX_IPV6_LEN    INET6_ADDRSTRLEN

int NK_inet_get_host_ipv4(const char *host_fqdn, char *ip_str, size_t ip_len);

