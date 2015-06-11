#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "aim.h"

#define VERTEX(g, v) (g->vertices[v])
#define EDGES(g, v) (g->edge_pool + VERTEX(g, v).edges_offset)

static inline struct vertex *
find_root(const struct graph * restrict g, uint v) {
	uint root = v;
	while (__builtin_expect(VERTEX(g, root).parent != root, true)) root = VERTEX(g, root).parent;
	struct vertex *vptr = &VERTEX(g, v);
	while (vptr->parent != root) {
		uint tmp = vptr->parent;
		vptr->parent = root;
		vptr = &VERTEX(g, tmp);
	}
	return &VERTEX(g, root);
}

static inline void
make_union(const struct graph * restrict g, struct vertex *a, struct vertex *b) {
	a->rank += a->rank == b->rank;
	if (b->rank > a->rank)
		a->parent = b - g->vertices;
	else
		b->parent = a - g->vertices;
}

struct k_edge {
	int32_t cost;
	uint a, b;
};

#define TYPE struct k_edge
#define GET(x) ((x).cost)
#include "sort.h"
#undef TYPE
#undef GET



/**
 * Kruskal's algorithm
 */
uint exercise(struct graph * restrict g, struct graph * restrict h) {
	uint ec = g->m / 2;
	struct k_edge * edges = malloc(sizeof(struct k_edge) * ec);
	struct k_edge * it = edges;
	for (long int i = 0; i < g->n; i++) {
		for (uint j = 0; j < g->vertices[i].edge_count; j++) {
			if (i < EDGES(g, i)[j].neighbour) {
				it->a = i;
				it->b = EDGES(g, i)[j].neighbour;
				it->cost = EDGES(g, i)[j].cost;
				it++;
			}
		}
	}

	radixsort(edges, ec);

	for (uint i = 0; i < ec; i++) {
		struct vertex * a = find_root(g, edges[i].a);
		struct vertex * b = find_root(g, edges[i].b);
		if (a == b) continue;

		make_union(g, a, b);

		struct edge * a_edge = &EDGES(h, edges[i].a)[VERTEX(h, edges[i].a).edge_count];
		struct edge * b_edge = &EDGES(h, edges[i].b)[VERTEX(h, edges[i].b).edge_count];

		a_edge->neighbour = edges[i].a;
		b_edge->neighbour = edges[i].b;
		b_edge->cost = a_edge->cost = edges[i].cost;
		VERTEX(h, edges[i].a).edge_count++;
		VERTEX(h, edges[i].b).edge_count++;
		h->m += 2;

		if (h->m / 2 == g->n - 1) break; // We already have spannning tree
	}
	return 0;
}
