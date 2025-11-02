#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include "dijkstra.h"
#include "graph.h"

/* ---------- minimal binary heap for bidirectional Dijkstra ---------- */
typedef struct {
    double key;
    int node;
} HeapItem;

typedef struct {
    HeapItem *a;
    int cap;
    int size;
} MinHeap;

static MinHeap* heap_create(int cap) {
    MinHeap* h = malloc(sizeof(MinHeap));
    h->cap = (cap > 0) ? cap : 1024;
    h->a = malloc(sizeof(HeapItem) * (h->cap + 1));
    h->size = 0;
    return h;
}

static void heap_free(MinHeap* h) {
    free(h->a);
    free(h);
}

static void heap_push(MinHeap* h, double key, int node) {
    if (h->size + 1 > h->cap) {
        h->cap *= 2;
        h->a = realloc(h->a, sizeof(HeapItem) * (h->cap + 1));
    }
    int i = ++h->size;
    while (i > 1 && h->a[i/2].key > key) {
        h->a[i] = h->a[i/2];
        i /= 2;
    }
    h->a[i].key = key;
    h->a[i].node = node;
}

static int heap_pop(MinHeap* h, double *key, int *node) {
    if (h->size == 0) return 0;
    *key = h->a[1].key;
    *node = h->a[1].node;
    HeapItem last = h->a[h->size--];
    int i = 1, c = 2;
    while (c <= h->size) {
        if (c + 1 <= h->size && h->a[c+1].key < h->a[c].key) c++;
        if (last.key <= h->a[c].key) break;
        h->a[i] = h->a[c];
        i = c;
        c = 2*i;
    }
    h->a[i] = last;
    return 1;
}

/* ---------- wrappers for Fibonacci / Pairing heap Dijkstra ---------- */
void dijkstra_fib(const Graph *g, int s, double *dist);
void dijkstra_pair(const Graph *g, int s, double *dist);

double* dijkstra_fibheap(const Graph *g, int s) {
    double* dist = malloc(g->num_nodes * sizeof(double));
    dijkstra_fib(g, s, dist);
    return dist;
}

double* dijkstra_pairingheap(const Graph *g, int s) {
    double* dist = malloc(g->num_nodes * sizeof(double));
    dijkstra_pair(g, s, dist);
    return dist;
}

/* ---------- Bidirectional Dijkstra ---------- */
double dijkstra_bi_one_query(const Graph *g, int s, int t, int *out_forward_visited, int *out_backward_visited) {
    if (s == t) {
        if (out_forward_visited) *out_forward_visited = 0;
        if (out_backward_visited) *out_backward_visited = 0;
        return 0.0;
    }

    int n = g->num_nodes;
    double *distF = malloc(sizeof(double) * n);
    double *distB = malloc(sizeof(double) * n);
    char *visF = malloc(n);
    char *visB = malloc(n);
    for (int i = 0; i < n; i++) {
        distF[i] = DBL_MAX;
        distB[i] = DBL_MAX;
        visF[i] = 0;
        visB[i] = 0;
    }

    MinHeap *hf = heap_create(1024);
    MinHeap *hb = heap_create(1024);

    distF[s] = 0.0;
    distB[t] = 0.0;
    heap_push(hf, 0.0, s);
    heap_push(hb, 0.0, t);

    double best = DBL_MAX;
    int expandedF = 0, expandedB = 0;

    while (hf->size > 0 || hb->size > 0) {
        // Forward step
        if (hf->size > 0) {
            double key; int u;
            while (hf->size > 0) {
                if (!heap_pop(hf, &key, &u)) break;
                if (key > distF[u]) continue;
                break;
            }
            if (key <= distF[u] && key != DBL_MAX) {
                if (!visF[u]) {
                    visF[u] = 1;
                    expandedF++;
                    for (Edge *e = g->adj[u]; e; e = e->next) {
                        int v = e->to;
                        double nd = distF[u] + e->weight;
                        if (nd < distF[v]) {
                            distF[v] = nd;
                            heap_push(hf, nd, v);
                        }
                    }
                }
            }
        }

        // Backward step
        if (hb->size > 0) {
            double key; int u;
            while (hb->size > 0) {
                if (!heap_pop(hb, &key, &u)) break;
                if (key > distB[u]) continue;
                break;
            }
            if (key <= distB[u] && key != DBL_MAX) {
                if (!visB[u]) {
                    visB[u] = 1;
                    expandedB++;
                    for (Edge *e = g->rev_adj[u]; e; e = e->next) {
                        int v = e->to;
                        double nd = distB[u] + e->weight;
                        if (nd < distB[v]) {
                            distB[v] = nd;
                            heap_push(hb, nd, v);
                        }
                    }
                }
            }
        }

        // Termination
        double topF = (hf->size > 0) ? hf->a[1].key : DBL_MAX;
        double topB = (hb->size > 0) ? hb->a[1].key : DBL_MAX;
        if (topF + topB >= best) break;

        // Update best
        for (int i = 0; i < n; i++) {
            if (distF[i] < DBL_MAX && distB[i] < DBL_MAX) {
                double cand = distF[i] + distB[i];
                if (cand < best) best = cand;
            }
        }
    }

    if (out_forward_visited) *out_forward_visited = expandedF;
    if (out_backward_visited) *out_backward_visited = expandedB;

    heap_free(hf);
    heap_free(hb);
    free(distF);
    free(distB);
    free(visF);
    free(visB);

    return best;
}

/* ---------- Fibonacci heap Dijkstra ---------- */
void dijkstra_fib(const Graph *g, int s, double *dist) {
    for (int i = 0; i < g->num_nodes; i++) dist[i] = DBL_MAX;
    FibHeap *H = fib_create(g->num_nodes);
    dist[s] = 0.0;
    fib_insert(H, 0.0, s);

    while (!fib_is_empty(H)) {  // 使用公共API而不是直接访问H->min
        int u = fib_extract_min(H);
        if (u == -1) break;
        
        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->to;
            double nd = dist[u] + e->weight;
            if (nd < dist[v]) {
                dist[v] = nd;
                // 这里需要检查节点是否已经在堆中，但fib_decrease_key内部会处理
                fib_decrease_key(H, v, nd);
            }
        }
    }
    fib_free(H);
}

/* ---------- Pairing heap Dijkstra ---------- */
/* ---------- Pairing heap Dijkstra ---------- */
void dijkstra_pair(const Graph *g, int s, double *dist) {
    int n = g->num_nodes;
    for (int i = 0; i < n; i++) dist[i] = DBL_MAX;

    PairingHeap *H = pair_create(n);
    dist[s] = 0.0;
    pair_insert(H, 0.0, s);

    while (H->root) {
        int u = pair_extract_min(H);
        if (u == -1) break;

        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->to;
            double nd = dist[u] + e->weight;
            if (nd < dist[v]) {
                dist[v] = nd;
                pair_decrease_key(H, v, nd);
            }
        }
    }

    pair_free(H);
}
