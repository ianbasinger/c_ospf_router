#include "../include/graph.h"
#include <stdio.h>

void graph_init(Graph *g, int nodes) {
    g->node_count = nodes;
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            g->cost[i][j] = (i == j) ? 0 : INF;
        }
    }
}

void graph_add_link(Graph *g, int a, int b, int cost) {
    if (a < 0 || b < 0 || a >= g->node_count || b >= g->node_count) return;
    g->cost[a][b] = cost;
    g->cost[b][a] = cost;
}

void graph_set_link_down(Graph *g, int a, int b) {
    if (a < 0 || b < 0 || a >= g->node_count || b >= g->node_count) return;
    g->cost[a][b] = INF;
    g->cost[b][a] = INF;
}

void graph_set_link_up(Graph *g, int a, int b, int cost) {
    graph_add_link(g, a, b, cost);
}

void graph_print(const Graph *g) {
    printf("Topology (%d nodes):\n", g->node_count);
    for (int i = 0; i < g->node_count; i++) {
        for (int j = 0; j < g->node_count; j++) {
            if (i == j) continue;
            if (g->cost[i][j] >= INF) continue;
            printf("  %d --(%d)--> %d\n", i, g->cost[i][j], j);
        }
    }
}