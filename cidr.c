#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/cidr.h"
#include "../include/util.h"

int parse_cidr(const char *cidr_str, CIDR *out) {
    char ip_part[32];
    char *slash = strchr(cidr_str, '/');
    if (!slash) return -1;

    size_t ip_len = slash - cidr_str;
    strncpy(ip_part, cidr_str, ip_len);
    ip_part[ip_len] = '\0';

    int prefix = atoi(slash + 1);
    if (prefix < 0 || prefix > 32) return -1;

    uint32_t ip = ip_to_uint32(ip_part);
    if (ip == 0 && strcmp(ip_part, "0.0.0.0") != 0) return -1;

    uint32_t mask = prefix_to_mask(prefix);
    uint32_t network = get_network(ip, mask);

    out->network = network;
    out->mask = mask;
    out->prefix = prefix;

    return 0;
}