#ifndef ROUTE_TABLE_H
#define ROUTE_TABLE_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    ROUTE_STATIC = 0,
    ROUTE_OSPF = 1
} RouteSource;

typedef struct {
    uint32_t network;
    uint32_t mask;
    uint8_t prefix;
    uint32_t next_hop;
    int metric;
    RouteSource source;
} Route;

typedef struct {
    Route *routes;
    size_t count;
    size_t capacity;
} RouteTable;

void route_table_init(RouteTable *rt, size_t initial_capacity);
void route_table_free(RouteTable *rt);

int route_table_add(RouteTable *rt, Route r);
int route_table_remove(RouteTable *rt, const char *cidr_str);
void route_table_print(const RouteTable *rt);

void route_table_remove_by_source(RouteTable *rt, RouteSource src);

#endif