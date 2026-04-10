#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NODES 32
#define INF 1000000000

typedef struct {
    int cost[MAX_NODES][MAX_NODES];
    int node_count;
} Graph;

void graph_init(Graph *g, int nodes);
void graph_add_link(Graph *g, int a, int b, int cost);
void graph_set_link_down(Graph *g, int a, int b);
void graph_set_link_up(Graph *g, int a, int b, int cost);
void graph_print(const Graph *g);

#endif