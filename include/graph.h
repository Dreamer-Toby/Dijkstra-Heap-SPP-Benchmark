#ifndef GRAPH_H
#define GRAPH_H

// =========================
// 图结构定义
// =========================

typedef struct Edge {
    int to;               // 目标节点
    double weight;        // 边权重（使用 double 以兼容浮点距离）
    struct Edge* next;    // 下一条边
} Edge;

typedef struct {
    int num_nodes;        // 节点总数
    long num_edges;       // 边总数
    Edge** adj;           // 正向邻接表
    Edge** rev_adj;       // 反向邻接表（用于双向Dijkstra）
} Graph;

// =========================
// 图的操作函数接口
// =========================

// 创建空图
Graph* create_graph(int num_nodes);

// 添加有向边
void add_edge(Graph* g, int u, int v, double weight);

// 从 DIMACS 文件加载图
Graph* load_dimacs_graph(const char* filename);

// 释放图内存
void free_graph(Graph* g);

#endif // GRAPH_H
