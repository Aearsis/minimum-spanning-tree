#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "aim.h"


#if CHAR_BIT != 8
#error char size larger than 8 is not supported
#endif


static struct graph *
graph_load(char *filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f) { perror("load"); exit(EXIT_FAILURE); }

	struct graph *g = calloc(1, sizeof(struct graph));

	/* First line contains number of vertices. */
	if (2 != fscanf(f, "%u %u\n", &g->n, &g->m)) {
		fprintf(stderr, "Format error: Invalid header.\n");
		goto fail;
	}
	g->vertices = calloc(g->n, sizeof(struct vertex));
	struct edge * edges = g->edge_pool = calloc(g->m, sizeof(struct edge));

	for (uint i = 0; i < g->n; i++) {
		uint neighbours;
		if (1 != fscanf(f, "%u", &neighbours)) {
			fprintf(stderr, "format error: invalid line %i.\n", i);
			goto fail;
		}
		g->vertices[i].edge_count = neighbours;
		g->vertices[i].edges = edges;
		for (uint j = 0; j < neighbours; j++) {
			uint vref;
			if (2 != fscanf(f, "%u %u", &vref, &edges[j].cost)) {
				fprintf(stderr, "Format error: Invalid edge %i at vertex %i.\n", j, i);
				goto fail;
			}
			if (vref >= g->n) {
				fprintf(stderr, "Format error: Invalid reference to nonexistent vertex %i.\n", vref);
				goto fail;
			}
			edges[j].neighbour = &g->vertices[vref];
		}
		edges += neighbours;
		if (edges - g->edge_pool > g->m + g->n) {
			fprintf(stderr, "Format error: Too many edges.\n");
			goto fail;
		}
	}
	fclose(f);
	graph_recalc(g);
	return g;
	fail:
	if (g) {
		if (g->vertices)
			free(g->vertices);
		if (g->edge_pool)
			free(g->edge_pool);
		free(g);
	}
	return NULL;
}

static void
graph_save(char *filename, struct graph *g)
{
	FILE *f = fopen(filename, "wb");
	if (!f) { perror("save"); exit(EXIT_FAILURE); }

	graph_recalc(g);
	fprintf(f, "%u %u\n", g->n, g->m);
	for (uint i = 0; i < g->n; i++) {
		struct edge * edges = g->vertices[i].edges;
		uint count = g->vertices[i].edge_count;
		fprintf(f, "%u ", count);
		if (count) {
			for (uint j = 0; j < count - 1; j++) {
				fprintf(f, "%ld %u ", (edges[j].neighbour - g->vertices), edges[j].cost);
			}
			fprintf(f, "%ld %u", (edges[count - 1].neighbour - g->vertices), edges[count - 1].cost);
		}
		fprintf(f, "\n");
	}

	fclose(f);
}

static struct graph *
graph_clone_vertices(struct graph *g)
{
	struct graph *h = calloc(1, sizeof(struct graph));
	h->n = g->n;
	h->vertices = calloc(g->n, sizeof(struct vertex));
	h->edge_pool = calloc(g->m, sizeof(struct edge));

	for (uint i = 0; i < g->n; i++) {
		h->vertices[i].edges = g->vertices[i].edges - g->edge_pool + h->edge_pool;
	}

	return h;
}

static struct graph *
graph_clone(struct graph *g)
{
	struct graph *h = graph_clone_vertices(g);
	h->m = g->m;

	for (uint i = 0; i < g->n; i++) {
		h->vertices[i].edge_count = g->vertices[i].edge_count;
		struct edge * g_edges = g->vertices[i].edges;
		struct edge * h_edges = h->vertices[i].edges;
		for (uint j = 0; j < h->vertices[i].edge_count; j++) {
			h_edges[j].cost = g_edges[j].cost;
			h_edges[j].neighbour = g_edges[j].neighbour - g->vertices + h->vertices;
		}
	}

	return h;
}

static void
graph_free(struct graph *g)
{
	free(g->edge_pool);
	free(g->vertices);
	free(g);
}


typedef int_fast64_t timestamp_t;

static timestamp_t
get_timer(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return 1000000*t.tv_sec + t.tv_usec;
}


int
main(int argc, char *argv[])
{
	if (argc != 4) {
		fprintf(stderr, "%s ITERATIONS SRC.g DST.g\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct graph *g_in = graph_load(argv[2]);
	timestamp_t t0 = get_timer();

	/* Pre-warm the CPU! Neccessary for benchmarking w/ dynamic
	 * cpufreq policy. */
	while (get_timer() - t0 < 500000);
	timestamp_t time = 0;

	int iters = atoi(argv[1]);
	for (int j = 0; j < iters; j++) {
		struct graph * g = graph_clone(g_in);
		struct graph * h = graph_clone_vertices(g_in);
		t0 = get_timer();

		exercise(g, h);

		time += get_timer() - t0;
		if (!j) graph_save(argv[3], h);
		graph_free(g);
		printf("%i: %.3f\n", j, (double) time/1e6);
	}

	// time spent:
	printf("%.3f\n", (double) time/1e6);
	graph_free(g_in);
	return EXIT_SUCCESS;
}
