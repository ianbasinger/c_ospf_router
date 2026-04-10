#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/route_table.h"
#include "../include/cidr.h"
#include "../include/util.h"

static int ensure_capacity(RouteTable *rt) {
    if (rt->count < rt->capacity) return 0;

    size_t new_cap = rt->capacity ? rt->capacity * 2 : 8;
    Route *new_routes = realloc(rt->routes, new_cap * sizeof(Route));
    if (!new_routes) return -1;

    rt->routes = new_routes;
    rt->capacity = new_cap;
    return 0;
}

void route_table_init(RouteTable *rt, size_t initial_capacity) {
rt->routes = malloc(sizeof(Route) * initial_capacity);
rt->count = 0;
rt->capacity = initial_capacity;
}

void route_table_free(RouteTable *rt) {
    if (!rt) return;
    free(rt->routes);
    rt->routes = NULL;
    rt->count = 0;
    rt->capacity = 0;
}

int route_table_add(RouteTable *rt, Route r) {
    if (!rt) return -1;
    if (ensure_capacity(rt) != 0) return -1;

    rt->routes[rt->count++] = r;
    return 0;
}

int route_table_remove(RouteTable *rt, const char *cidr_str) {
    if (!rt || !cidr_str) return -1;

    CIDR c;
    if (parse_cidr(cidr_str, &c) != 0) return -1;

    for (size_t i = 0; i < rt->count; i++) {
        Route *r = &rt->routes[i];
        if (r->network == c.network && r->prefix == c.prefix && r->mask == c.mask) {
            rt->routes[i] = rt->routes[rt->count - 1];
            rt->count--;
            return 0;
        }
    }

    return -1;
}

void route_table_print(const RouteTable *rt) {
    if (!rt) return;

    printf("Routes (%zu):\n", rt->count);
    for (size_t i = 0; i < rt->count; i++) {
        const Route *r = &rt->routes[i];

        char net[16];
        char nh[16];

        uint32_to_ip(r->network, net);
        uint32_to_ip(r->next_hop, nh);

        printf("  %s/%u via %s metric %d\n", net, r->prefix, nh, r->metric);
    }
}

static void route_table_compact(RouteTable *rt) {
size_t w = 0;
for (size_t r = 0; r < rt->count; r++) {
rt->routes[w++] = rt->routes[r];
}
rt->count = w;
}


void route_table_remove_by_source(RouteTable *rt, RouteSource src) {
size_t w = 0;
for (size_t i = 0; i < rt->count; i++) {
if (rt->routes[i].source != src) {
rt->routes[w++] = rt->routes[i];
}
}
rt->count = w;
}