#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "graph.h"

#ifdef _WIN32
#include <direct.h>
#define CREATE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define CREATE_DIR(path) mkdir(path, 0755)
#endif

/**
 * @brief 创建目录（如果不存在）
 * @param path 目录路径
 */
void create_directory(const char* path) {
#ifdef _WIN32
    _mkdir(path);
#else
    mkdir(path, 0755);
#endif
}

/**
 * @brief 生成空查询文件（用于测试空图情况）
 * @param folder 查询文件保存的文件夹路径
 */
void generate_empty_queries(const char* folder) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/empty_queries.txt", folder);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        // 空文件 - 没有查询对
        fclose(fp);
        printf("Generated empty queries: %s\n", filename);
    } else {
        printf("Failed to create empty queries file: %s\n", filename);
    }
}

/**
 * @brief 生成单节点图查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 */
void generate_single_node_queries(const char* folder, Graph* g) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/single_node_queries.txt", folder);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        if (g->num_nodes >= 1) {
            // 单个节点的自环查询
            fprintf(fp, "0 0\n");
            printf("Generated single node queries: %s\n", filename);
        } else {
            printf("Graph has no nodes, creating empty single node file\n");
        }
        fclose(fp);
    } else {
        printf("Failed to create single node queries file: %s\n", filename);
    }
}

/**
 * @brief 生成完全图查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_complete_graph_queries(const char* folder, Graph* g, int count) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/complete_graph_queries_%d.txt", folder, count);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        if (g->num_nodes >= 2) {
            srand(12345);
            int generated = 0;
            
            printf("Generating complete graph queries...\n");
            while (generated < count) {
                int s = rand() % g->num_nodes;
                int t = rand() % g->num_nodes;
                
                if (s != t) { // 避免自环
                    fprintf(fp, "%d %d\n", s, t);
                    generated++;
                    
                    if (generated % 100 == 0) {
                        printf("  Generated %d/%d complete graph queries...\n", generated, count);
                    }
                }
            }
            printf("Generated complete graph queries: %s (%d pairs)\n", filename, count);
        } else {
            printf("Graph has less than 2 nodes, creating empty complete graph file\n");
        }
        fclose(fp);
    } else {
        printf("Failed to create complete graph queries file: %s\n", filename);
    }
}

/**
 * @brief 生成正常情况查询文件（考虑图的稀疏性）
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_normal_queries(const char* folder, Graph* g, int count) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/normal_queries_%d.txt", folder, count);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        if (g->num_nodes >= 2) {
            srand(time(NULL)); // 使用时间种子，避免重复
            int generated = 0;
            int attempts = 0;
            const int MAX_ATTEMPTS = count * 100; // 增加尝试次数
            
            printf("Generating normal queries...\n");
            
            while (generated < count && attempts < MAX_ATTEMPTS) {
                int s = rand() % g->num_nodes;
                int t = rand() % g->num_nodes;
                attempts++;
                
                // 只需要确保节点索引有效且不是自环
                if (s != t) {
                    fprintf(fp, "%d %d\n", s, t);
                    generated++;
                    
                    if (generated % 1000 == 0) {
                        printf("  Generated %d/%d normal queries...\n", generated, count);
                    }
                }
            }
            printf("Generated normal queries: %s (%d pairs, %d attempts)\n", filename, generated, attempts);
            
            if (generated < count) {
                printf("Warning: Only generated %d out of %d queries (max attempts reached)\n", generated, count);
            }
        } else {
            printf("Graph has less than 2 nodes, creating empty normal queries file\n");
        }
        fclose(fp);
    } else {
        printf("Failed to create normal queries file: %s\n", filename);
    }
}

/**
 * @brief 生成大规模查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_large_scale_queries(const char* folder, Graph* g, int count) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/large_scale_queries_%d.txt", folder, count);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        if (g->num_nodes >= 2) {
            srand(time(NULL)); // 使用时间种子，避免重复
            int generated = 0;
            int attempts = 0;
            const int MAX_ATTEMPTS = count * 100; // 增加尝试次数
            
            printf("Generating large scale queries...\n");
            
            while (generated < count && attempts < MAX_ATTEMPTS) {
                int s = rand() % g->num_nodes;
                int t = rand() % g->num_nodes;
                attempts++;
                
                if (s != t) {
                    fprintf(fp, "%d %d\n", s, t);
                    generated++;
                    
                    if (generated % 1000 == 0) {
                        printf("  Generated %d/%d large scale queries...\n", generated, count);
                    }
                }
            }
            
            printf("Generated large scale queries: %s (%d pairs, %d attempts)\n", filename, generated, attempts);
            
            if (generated < count) {
                printf("Warning: Only generated %d out of %d queries (max attempts reached)\n", generated, count);
            }
        } else {
            printf("Graph has less than 2 nodes, creating empty large scale file\n");
        }
        fclose(fp);
    } else {
        printf("Failed to create large scale queries file: %s\n", filename);
    }
}

/**
 * @brief 生成小规模测试查询文件（使用预定义的节点对）
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 */
void generate_small_test_queries(const char* folder, Graph* g) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/small_test_queries_10.txt", folder);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        printf("Generating small test queries from predefined node pairs...\n");
        
        // 从你提供的节点对中选取10对
        int predefined_pairs[][2] = {
            {1, 2},
            {2, 1},
            {1048577, 1048578},
            {1048578, 1048577},
            {2097153, 3},
            {3, 2097153},
            {4, 1048579},
            {1048579, 4},
            {2097154, 5},
            {5, 2097154},
            {6, 1048580},
            {1048580, 6},
            {1048581, 7},
            {7, 1048581},
            {7, 8},
            {8, 7},
            {1048582, 1048580},
            {1048580, 1048582},
            {7, 1048583},
            {1048583, 7}
        };
        
        int total_pairs = sizeof(predefined_pairs) / sizeof(predefined_pairs[0]);
        int used[20] = {0}; // 标记已使用的对
        int generated = 0;
        
        srand(time(NULL));
        
        // 随机选择10对不重复的查询
        while (generated < 10 && generated < total_pairs) {
            int idx = rand() % total_pairs;
            if (!used[idx]) {
                int s = predefined_pairs[idx][0];
                int t = predefined_pairs[idx][1];
                
                // 检查节点是否在有效范围内
                if (s >= 0 && s < g->num_nodes && t >= 0 && t < g->num_nodes) {
                    fprintf(fp, "%d %d\n", s, t);
                    used[idx] = 1;
                    generated++;
                    printf("  Generated query %d: %d -> %d\n", generated, s, t);
                }
            }
        }
        
        // 如果预定义的节点对不够10个，用其他预定义对补充
        if (generated < 10) {
            printf("Adding more predefined pairs...\n");
            for (int i = 0; i < total_pairs && generated < 10; i++) {
                if (!used[i]) {
                    int s = predefined_pairs[i][0];
                    int t = predefined_pairs[i][1];
                    if (s >= 0 && s < g->num_nodes && t >= 0 && t < g->num_nodes) {
                        fprintf(fp, "%d %d\n", s, t);
                        used[i] = 1;
                        generated++;
                        printf("  Added query %d: %d -> %d\n", generated, s, t);
                    }
                }
            }
        }
        
        printf("Generated small test queries: %s (%d predefined pairs)\n", filename, generated);
        
        fclose(fp);
    } else {
        printf("Failed to create small test queries file: %s\n", filename);
    }
}

/**
 * @brief 生成边界情况查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 */
void generate_edge_case_queries(const char* folder, Graph* g) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/edge_case_queries.txt", folder);
    
    FILE* fp = fopen(filename, "w");
    if (fp) {
        if (g->num_nodes >= 1) {
            printf("Generating edge case queries...\n");
            
            // 各种边界情况
            fprintf(fp, "0 0\n");                    // 自环
            
            if (g->num_nodes >= 2) {
                fprintf(fp, "0 1\n");                    // 正常边
                fprintf(fp, "%d %d\n", g->num_nodes-1, 0); // 最大节点到最小节点
                fprintf(fp, "0 %d\n", g->num_nodes-1);   // 最小节点到最大节点
            }
            
            // 寻找孤立节点（没有出边的节点）
            int isolated_node = -1;
            for (int i = 0; i < g->num_nodes; i++) {
                if (g->adj[i] == NULL) {
                    isolated_node = i;
                    break;
                }
            }
            
            if (isolated_node != -1) {
                fprintf(fp, "%d 0\n", isolated_node);  // 从孤立节点出发
                fprintf(fp, "0 %d\n", isolated_node);  // 到孤立节点
                printf("Found isolated node: %d\n", isolated_node);
            } else {
                printf("No isolated nodes found\n");
            }
            
            // 寻找高度数节点
            int max_degree_node = 0;
            int max_degree = 0;
            for (int i = 0; i < g->num_nodes; i++) {
                if (g->adj[i] != NULL) {
                    int degree = 0;
                    Edge* e = g->adj[i];
                    while (e) {
                        degree++;
                        e = e->next;
                    }
                    if (degree > max_degree) {
                        max_degree = degree;
                        max_degree_node = i;
                    }
                }
            }
            
            if (max_degree > 0) {
                // 从高度数节点到它的一个邻居
                Edge* e = g->adj[max_degree_node];
                if (e != NULL) {
                    fprintf(fp, "%d %d\n", max_degree_node, e->to);
                }
                printf("Found high-degree node: %d (degree: %d)\n", max_degree_node, max_degree);
            }
            
            printf("Generated edge case queries: %s\n", filename);
        } else {
            printf("Graph has no nodes, creating empty edge case file\n");
        }
        fclose(fp);
    } else {
        printf("Failed to create edge case queries file: %s\n", filename);
    }
}

/**
 * @brief 验证查询文件的有效性
 * @param folder 查询文件所在的文件夹路径
 * @param filename 查询文件名
 * @param g 图结构指针（用于验证节点索引）
 * @return 有效查询对数量，-1表示文件打开失败
 */
int validate_query_file(const char* folder, const char* filename, Graph* g) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", folder, filename);
    
    FILE* fp = fopen(full_path, "r");
    if (!fp) {
        printf("Cannot open query file: %s\n", full_path);
        return -1;
    }
    
    int valid_pairs = 0;
    int total_pairs = 0;
    int s, t;
    
    while (fscanf(fp, "%d %d", &s, &t) == 2) {
        total_pairs++;
        if (s >= 0 && s < g->num_nodes && t >= 0 && t < g->num_nodes) {
            valid_pairs++;
        } else {
            printf("Invalid node indices in query %d: %d -> %d (graph has %d nodes)\n", 
                   total_pairs, s, t, g->num_nodes);
        }
    }
    
    fclose(fp);
    
    printf("Query file %s: %d/%d valid pairs\n", filename, valid_pairs, total_pairs);
    return valid_pairs;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <graph_file> <queries_folder>\n", argv[0]);
        printf("Example: %s data/USA-road-d.USA.gr Queries\n", argv[0]);
        return 1;
    }
    
    const char* graph_file = argv[1];
    const char* queries_folder = argv[2];
    
    // 创建查询文件夹
    create_directory(queries_folder);
    
    printf("Loading graph: %s\n", graph_file);
    Graph* g = load_dimacs_graph(graph_file);
    if (!g) {
        fprintf(stderr, "Error: failed to load graph file.\n");
        return 1;
    }
    
    printf("Graph loaded successfully: %d nodes\n", g->num_nodes);
    
    // 生成各种测试情况的查询文件
    printf("\nGenerating query files in folder: %s\n", queries_folder);
    printf("==========================================\n");
    
    generate_empty_queries(queries_folder);
    generate_single_node_queries(queries_folder, g);
    generate_small_test_queries(queries_folder, g);  // 现在使用相邻节点生成
    generate_complete_graph_queries(queries_folder, g, 100);
    generate_normal_queries(queries_folder, g, 1000);
    generate_large_scale_queries(queries_folder, g, 10000);
    generate_edge_case_queries(queries_folder, g);
    
    printf("\n==========================================\n");
    printf("All query files generated successfully!\n");
    
    // 验证生成的查询文件
    printf("\nValidating generated query files:\n");
    validate_query_file(queries_folder, "normal_queries_1000.txt", g);
    validate_query_file(queries_folder, "large_scale_queries_10000.txt", g);
    validate_query_file(queries_folder, "small_test_queries_10.txt", g);
    
    free_graph(g);
    return 0;
}