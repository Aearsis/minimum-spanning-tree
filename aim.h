#ifndef AIM__AIM_H
#define AIM__AIM_H

#include <stdbool.h>

typedef unsigned int uint;

struct edge {
	struct vertex * neighbour;
	uint cost;
};

struct vertex {
	struct edge * edges;
	struct vertex * parent;
	unsigned int rank;
	uint edge_count;
};

struct graph {
	uint n, m;
	struct edge *edge_pool;
	struct vertex * vertices;
};

uint exercise(struct graph * restrict in, struct graph * restrict out);


/** Implementation: */
static inline void
graph_recalc(struct graph * g) {
	g->m = 0;
	for (uint i = 0; i < g->n; i++)
		g->m += g->vertices[i].edge_count;
}

#endif
