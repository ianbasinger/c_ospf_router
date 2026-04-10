#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

uint32_t ip_to_uint32(const char *);
void uint32_to_ip(uint32_t, char *);
uint32_t prefix_to_mask(uint8_t);
uint32_t get_network(uint32_t, uint32_t);

int count_bits(uint32_t x);

#endif