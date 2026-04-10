#include <stdio.h>
#include "../include/route_table.h"
#include "../include/graph.h"
#include "../include/ospf.h"
#include "../include/util.h"
#include "../include/router_cli.h"

int main() {
    RouteTable rt;
    route_table_init(&rt, 8);

    RouterCtx ctx;
    ctx.rt = &rt;
    ctx.topo_ready = 0;
    ctx.ospf_source = 0;
    ctx.ospf_auto = 0;

    router_cli_loop(&ctx);

    route_table_free(&rt);
    return 0;
}