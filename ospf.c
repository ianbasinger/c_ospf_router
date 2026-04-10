#include "../include/ospf.h"
#include "graph.h"
#include "../include/util.h"
#include <limits.h>

void ospf_run(Graph *g, int source, RouteTable *rt) {
    int dist[MAX_NODES];
    int prev[MAX_NODES];
    int visited[MAX_NODES];

    int n = g->node_count;

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = 0;
    }

    dist[source] = 0;

    for (int i = 0; i < n; i++) {
        int u = -1;
        for (int j = 0; j < n; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (g->cost[u][v] < INF) {
                int alt = dist[u] + g->cost[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }

    for (int dest = 0; dest < n; dest++) {
        if (dest == source || dist[dest] == INF) continue;

        int next = dest;
        while (prev[next] != -1 && prev[next] != source) {
            next = prev[next];
        }

        Route r;
        r.network = dest;
        r.mask = 0xFFFFFFFF;
        r.prefix = 32;
        r.next_hop = next;
        r.metric = dist[dest];

        route_table_add(rt, r);
    }
}