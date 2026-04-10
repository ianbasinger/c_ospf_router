#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/router_cli.h"
#include "../include/util.h"
#include "../include/cidr.h"
#include "../include/lookup.h"
#include "../include/ospf.h"

static void print_help(void) {
    printf("\nCommands:\n");
    printf("  help | ?\n");
    printf("  show\n");
    printf("  lookup <ip>\n");
    printf("  add <cidr> via <ip> metric <n>\n");
    printf("  add <network> <mask> via <ip> metric <n>\n");
    printf("  del <cidr>\n");
    printf("\nTopology:\n");
    printf("  topo init <n>\n");
    printf("  topo link <a> <b> <cost>\n");
    printf("  topo down <a> <b>\n");
    printf("  topo up <a> <b> <cost>\n");
    printf("  topo show\n");
    printf("\nOSPF:\n");
    printf("  ospf run [source]\n");
    printf("  ospf auto on|off\n");
    printf("  ospf show\n");
    printf("  ospf fail <a> <b>\n");
    printf("\nExit:\n");
    printf("  exit\n\n");

    printf("Examples:\n");
    printf("  add 192.168.1.0/24 via 192.168.1.1 metric 10\n");
    printf("  add 192.168.2.0 255.255.255.0 via 192.168.1.254 metric 5\n");
    printf("  topo init 4\n");
    printf("  topo link 0 1 10\n");
    printf("  topo link 1 2 5\n");
    printf("  topo link 2 3 2\n");
    printf("  ospf run 0\n");
    printf("  ospf auto on\n");
    printf("  topo down 1 2\n");
    printf("  ospf show\n");
}

static void ospf_recompute(RouterCtx *ctx) {
    if (!ctx->topo_ready) {
        printf("Topology not initialized. Use: topo init <n>\n");
        return;
    }
    route_table_remove_by_source(ctx->rt, ROUTE_OSPF);
    ospf_run(&ctx->topo, ctx->ospf_source, ctx->rt);
    printf("OSPF recomputed from source %d.\n", ctx->ospf_source);
}

static Route make_route_cidr(const char *cidr, const char *next_hop, int metric) {
    CIDR c;
    Route r;

    if (parse_cidr(cidr, &c) != 0) {
        r.prefix = 255;
        return r;
    }

    r.network = c.network;
    r.mask = c.mask;
    r.prefix = c.prefix;
    r.next_hop = ip_to_uint32(next_hop);
    r.metric = metric;
    r.source = ROUTE_STATIC;

    return r;
}

static void handle_add(RouteTable *rt, const char *args) {
    char net[32], mask[32], via[16];
    int metric;

    if (sscanf(args, "%31s via %15s metric %d", net, via, &metric) == 3) {
        Route r = make_route_cidr(net, via, metric);
        if (r.prefix != 255) {
            route_table_add(rt, r);
            printf("Route added.\n");
        } else {
            printf("Invalid CIDR.\n");
        }
        return;
    }

    if (sscanf(args, "%31s %31s via %15s metric %d", net, mask, via, &metric) == 4) {
        uint32_t network = ip_to_uint32(net);
        uint32_t netmask = ip_to_uint32(mask);

        Route r;
        r.network = network & netmask;
        r.mask = netmask;
        r.prefix = count_bits(netmask);
        r.next_hop = ip_to_uint32(via);
        r.metric = metric;
        r.source = ROUTE_STATIC;

        route_table_add(rt, r);
        printf("Route added.\n");
        return;
    }

    printf("Invalid add syntax. Type 'help'.\n");
}

static void handle_lookup(RouteTable *rt, const char *ip_str) {
    uint32_t ip = ip_to_uint32(ip_str);
    const Route *r = lookup_route(rt, ip);

    if (r) {
        char net[16], nh[16];
        uint32_to_ip(r->network, net);
        uint32_to_ip(r->next_hop, nh);
        printf("%s/%u via %s metric %d\n", net, r->prefix, nh, r->metric);
    } else {
        printf("No route found.\n");
    }
}

static void handle_topo(RouterCtx *ctx, const char *args) {
    char sub[16];
    int a, b, cost, n;

    if (sscanf(args, "%15s", sub) != 1) {
        printf("Usage: topo init|link|down|up|show ...\n");
        return;
    }

    if (strcmp(sub, "init") == 0) {
        if (sscanf(args, "init %d", &n) == 1 && n > 0 && n <= MAX_NODES) {
            graph_init(&ctx->topo, n);
            ctx->topo_ready = 1;
            if (ctx->ospf_source < 0 || ctx->ospf_source >= n) ctx->ospf_source = 0;
            printf("Topology initialized with %d nodes.\n", n);
            if (ctx->ospf_auto) ospf_recompute(ctx);
        } else {
            printf("Usage: topo init <n> (1..%d)\n", MAX_NODES);
        }
        return;
    }

    if (!ctx->topo_ready) {
        printf("Topology not initialized. Use: topo init <n>\n");
        return;
    }

    if (strcmp(sub, "link") == 0) {
        if (sscanf(args, "link %d %d %d", &a, &b, &cost) == 3) {
            graph_add_link(&ctx->topo, a, b, cost);
            printf("Link set: %d <-> %d cost %d\n", a, b, cost);
            if (ctx->ospf_auto) ospf_recompute(ctx);
        } else {
            printf("Usage: topo link <a> <b> <cost>\n");
        }
        return;
    }

    if (strcmp(sub, "down") == 0) {
        if (sscanf(args, "down %d %d", &a, &b) == 2) {
            graph_set_link_down(&ctx->topo, a, b);
            printf("Link down: %d <-> %d\n", a, b);
            if (ctx->ospf_auto) ospf_recompute(ctx);
        } else {
            printf("Usage: topo down <a> <b>\n");
        }
        return;
    }

    if (strcmp(sub, "up") == 0) {
        if (sscanf(args, "up %d %d %d", &a, &b, &cost) == 3) {
            graph_set_link_up(&ctx->topo, a, b, cost);
            printf("Link up: %d <-> %d cost %d\n", a, b, cost);
            if (ctx->ospf_auto) ospf_recompute(ctx);
        } else {
            printf("Usage: topo up <a> <b> <cost>\n");
        }
        return;
    }

    if (strcmp(sub, "show") == 0) {
        graph_print(&ctx->topo);
        return;
    }

    printf("Unknown topo subcommand. Type 'help'.\n");
}

static void handle_ospf(RouterCtx *ctx, const char *args) {
    char sub[16], onoff[8];
    int a, b, src;

    if (sscanf(args, "%15s", sub) != 1) {
        printf("Usage: ospf run|auto|show|fail ...\n");
        return;
    }

    if (strcmp(sub, "run") == 0) {
        if (sscanf(args, "run %d", &src) == 1) ctx->ospf_source = src;
        ospf_recompute(ctx);
        return;
    }

    if (strcmp(sub, "auto") == 0) {
        if (sscanf(args, "auto %7s", onoff) == 1) {
            if (strcmp(onoff, "on") == 0) {
                ctx->ospf_auto = 1;
                printf("OSPF auto reconvergence: ON\n");
                if (ctx->topo_ready) ospf_recompute(ctx);
            } else if (strcmp(onoff, "off") == 0) {
                ctx->ospf_auto = 0;
                printf("OSPF auto reconvergence: OFF\n");
            } else {
                printf("Usage: ospf auto on|off\n");
            }
        } else {
            printf("Usage: ospf auto on|off\n");
        }
        return;
    }

    if (strcmp(sub, "show") == 0) {
        route_table_print(ctx->rt);
        return;
    }

    if (strcmp(sub, "fail") == 0) {
        if (sscanf(args, "fail %d %d", &a, &b) == 2) {
            if (!ctx->topo_ready) {
                printf("Topology not initialized. Use: topo init <n>\n");
                return;
            }
            graph_set_link_down(&ctx->topo, a, b);
            printf("Simulated failure: %d <-> %d\n", a, b);
            if (ctx->ospf_auto) ospf_recompute(ctx);
        } else {
            printf("Usage: ospf fail <a> <b>\n");
        }
        return;
    }

    printf("Unknown ospf subcommand. Type 'help'.\n");
}

void router_cli_loop(RouterCtx *ctx) {
    char line[256];

    print_help();

    while (1) {
        printf("router> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "help") == 0 || strcmp(line, "?") == 0) {
            print_help();
        }
        else if (strcmp(line, "show") == 0) {
            route_table_print(ctx->rt);
        }
        else if (strncmp(line, "add ", 4) == 0) {
            handle_add(ctx->rt, line + 4);
        }
        else if (strncmp(line, "del ", 4) == 0) {
            if (route_table_remove(ctx->rt, line + 4) == 0)
                printf("Route removed.\n");
            else
                printf("Route not found.\n");
        }
        else if (strncmp(line, "lookup ", 7) == 0) {
            handle_lookup(ctx->rt, line + 7);
        }
        else if (strncmp(line, "topo ", 5) == 0) {
            handle_topo(ctx, line + 5);
        }
        else if (strncmp(line, "ospf ", 5) == 0) {
            handle_ospf(ctx, line + 5);
        }
        else if (strcmp(line, "exit") == 0) {
            break;
        }
        else if (strlen(line) > 0) {
            printf("Unknown command. Type 'help'.\n");
        }
    }
}