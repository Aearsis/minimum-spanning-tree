#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "aim.h"

static inline struct vertex *
find_root(struct vertex * v) {
	struct vertex *root = v;
	while (root->parent) root = root->parent;
	while (v->parent) {
		struct vertex *tmp = v->parent;
		v->parent = root;
		v = tmp;
	}
	return root;
}

static inline void
make_union(struct vertex *a, struct vertex *b) {
	a->rank += a->rank == b->rank;
	b->parent = a;
}

struct k_edge {
	int32_t cost;
	struct vertex *a, *b;
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
			if (&g->vertices[i] < g->vertices[i].edges[j].neighbour) {
				it->a = &g->vertices[i];
				it->b = g->vertices[i].edges[j].neighbour;
				it->cost = g->vertices[i].edges[j].cost;
				it++;
			}
		}
	}

	radixsort(edges, ec);

	for (uint i = 0; i < ec; i++) {
		struct vertex * a = find_root(edges[i].a);
		struct vertex * b = find_root(edges[i].b);
		if (a == b) continue;
		
		if (b->rank > a->rank)
			make_union(b, a);
		else
			make_union(a, b);

		struct vertex * h_a = edges[i].a - g->vertices + h->vertices;
		struct vertex * h_b = edges[i].b - g->vertices + h->vertices;
		struct edge * a_edge = &h_a->edges[h_a->edge_count];
		struct edge * b_edge = &h_b->edges[h_b->edge_count];

		a_edge->neighbour = h_b;
		b_edge->neighbour = h_a;
		b_edge->cost = a_edge->cost = edges[i].cost;
		h_a->edge_count++;
		h_b->edge_count++;
		h->m += 2;

		if (h->m / 2 == g->n - 1) break; // We already have spannning tree
	}
	return 0;
}
