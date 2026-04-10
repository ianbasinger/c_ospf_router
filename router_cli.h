#ifndef ROUTER_CLI_H
#define ROUTER_CLI_H

#include "route_table.h"
#include "graph.h"

typedef struct {
    RouteTable *rt;
    Graph topo;
    int topo_ready;

    int ospf_source;
    int ospf_auto;
} RouterCtx;

void router_cli_loop(RouterCtx *ctx);

#endif