#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

// =========================
// 辅助函数：向邻接表添加一条边
// =========================
static void add_edge_to_list(Edge** adj, int from, int to, double weight) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    if (!e) {
        fprintf(stderr, "Error: failed to allocate edge.\n");
        exit(EXIT_FAILURE);
    }
    e->to = to;
    e->weight = weight;
    e->next = adj[from];
    adj[from] = e;
}

// =========================
// 创建空图
// =========================
Graph* create_graph(int num_nodes) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Error: failed to allocate Graph structure.\n");
        exit(EXIT_FAILURE);
    }

    g->num_nodes = num_nodes;
    g->num_edges = 0;
    g->adj = (Edge**)calloc(num_nodes, sizeof(Edge*));
    g->rev_adj = (Edge**)calloc(num_nodes, sizeof(Edge*));

    if (!g->adj || !g->rev_adj) {
        fprintf(stderr, "Error: failed to allocate adjacency lists.\n");
        free(g->adj);
        free(g->rev_adj);
        free(g);
        exit(EXIT_FAILURE);
    }

    return g;
}

// =========================
// 添加有向边（仅单向）
// =========================
void add_edge(Graph* g, int u, int v, double weight) {
    if (u < 0 || v < 0 || u >= g->num_nodes || v >= g->num_nodes)
        return;

    add_edge_to_list(g->adj, u, v, weight);
    g->num_edges++;
}

// =========================
// 释放图内存
// =========================
void free_graph(Graph* g) {
    if (!g) return;
    for (int i = 0; i < g->num_nodes; i++) {
        Edge* e = g->adj[i];
        while (e) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
        Edge* r = g->rev_adj[i];
        while (r) {
            Edge* next = r->next;
            free(r);
            r = next;
        }
    }
    free(g->adj);
    free(g->rev_adj);
    free(g);
}

// =========================
// 从 DIMACS 文件加载图
// =========================
Graph* load_dimacs_graph(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return NULL;
    }

    printf("Reading DIMACS graph file: %s\n", filename);

    int num_nodes = 0, num_edges = 0;
    char line[256];

    // === 解析 "p sp NODES EDGES" 行 ===
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'p') {
            if (sscanf(line, "p sp %d %d", &num_nodes, &num_edges) == 2)
                break;
        }
    }

    if (num_nodes <= 0) {
        fprintf(stderr, "Error: missing 'p sp' header line in %s\n", filename);
        fclose(fp);
        return NULL;
    }

    printf("Allocating graph with %d nodes (≈%d edges expected)...\n", num_nodes, num_edges);

    Graph* g = create_graph(num_nodes);
    if (!g) {
        fclose(fp);
        return NULL;
    }

    long edge_count = 0;
    int from, to;
    double weight;

    // === 读取每条边 ===
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != 'a') continue; // 跳过注释或空行

        if (sscanf(line, "a %d %d %lf", &from, &to, &weight) == 3) {
            from -= 1;  // DIMACS 是 1-based 编号
            to -= 1;

            if (from >= 0 && from < num_nodes && to >= 0 && to < num_nodes) {
                // 正向边：from -> to
                add_edge_to_list(g->adj, from, to, weight);
                // 反向边：to -> from，用于反向搜索
                add_edge_to_list(g->rev_adj, to, from, weight);
                edge_count++;
            }
        }

        if (edge_count % 5000000 == 0 && edge_count > 0) {
#ifdef _WIN32
            printf("  Loaded %ld edges...\n", edge_count);
#else
            printf("  Loaded %ld edges...\n", edge_count);
#endif
            fflush(stdout);
        }
    }

#ifdef _WIN32
    printf("Finished loading graph: %ld edges read.\n", edge_count);
#else
    printf("Finished loading graph: %ld edges read.\n", edge_count);
#endif

    fclose(fp);
    return g;
}
