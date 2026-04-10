#ifndef OSPF_H
#define OSPF_H

#include "graph.h"
#include "route_table.h"

void ospf_run(Graph *g, int source, RouteTable *rt);

#endif