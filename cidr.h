#ifndef CIDR_H
#define CIDR_H

#include <stdint.h>

typedef struct {
    uint32_t network;
    uint32_t mask;
    uint8_t prefix;
} CIDR;

int parse_cidr(const char *cidr_str, CIDR *out);

#endif