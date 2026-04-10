#include "../include/lookup.h"

const Route* lookup_route(const RouteTable *rt, uint32_t dest_ip) {
    if (!rt || rt->count == 0) return NULL;

    const Route *best = NULL;

    for (size_t i = 0; i < rt->count; i++) {
        const Route *r = &rt->routes[i];

        if ((dest_ip & r->mask) == r->network) {
            if (!best ||
                r->prefix > best->prefix ||
                (r->prefix == best->prefix && r->metric < best->metric)) {
                best = r;
            }
        }
    }

    return best;
}