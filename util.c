#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif


#include "../include/util.h"

uint32_t ip_to_uint32(const char *ip_str) {
    struct in_addr addr;
    if (inet_pton(AF_INET, ip_str, &addr) != 1) {
        return 0;
    }
    return ntohl(addr.s_addr);
}

void uint32_to_ip(uint32_t ip, char *buffer) {
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, buffer, INET_ADDRSTRLEN);
}

uint32_t prefix_to_mask(uint8_t prefix) {
    if (prefix == 0) return 0;
    return 0xFFFFFFFF << (32 - prefix);
}

uint32_t get_network(uint32_t ip, uint32_t mask) {
    return ip & mask;
}

int count_bits(uint32_t x) {
	int count = 0;
	while (x) {
	count += x & 1;
	x >>= 1;
	}
	return count;
}