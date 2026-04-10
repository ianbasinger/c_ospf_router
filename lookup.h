#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdint.h>
#include "route_table.h"

const Route* lookup_route(const RouteTable *rt, uint32_t dest_ip);

#endif