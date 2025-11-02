// generate_queries.h
#ifndef GENERATE_QUERIES_H
#define GENERATE_QUERIES_H

#include "graph.h"

/**
 * @brief 创建目录（如果不存在）
 * @param path 目录路径
 */
void create_directory(const char* path);

/**
 * @brief 生成空查询文件（用于测试空图情况）
 * @param folder 查询文件保存的文件夹路径
 */
void generate_empty_queries(const char* folder);

/**
 * @brief 生成单节点图查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 */
void generate_single_node_queries(const char* folder, Graph* g);

/**
 * @brief 生成完全图查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_complete_graph_queries(const char* folder, Graph* g, int count);

/**
 * @brief 生成正常情况查询文件（考虑图的稀疏性）
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_normal_queries(const char* folder, Graph* g, int count);

/**
 * @brief 生成大规模查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 */
void generate_large_scale_queries(const char* folder, Graph* g, int count);

/**
 * @brief 生成边界情况查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 */
void generate_edge_case_queries(const char* folder, Graph* g);

/**
 * @brief 生成指定数量的随机查询文件
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 * @param filename 输出文件名
 * @param seed 随机数种子（用于可重复性）
 */
void generate_custom_queries(const char* folder, Graph* g, int count, 
                            const char* filename, unsigned int seed);

/**
 * @brief 生成基于节点度的查询文件（偏向高度数节点）
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 * @param degree_weighted 是否根据节点度进行加权采样
 */
void generate_degree_weighted_queries(const char* folder, Graph* g, 
                                     int count, int degree_weighted);

/**
 * @brief 生成地理分布的查询文件（用于空间数据）
 * @param folder 查询文件保存的文件夹路径
 * @param g 图结构指针
 * @param count 要生成的查询对数量
 * @param max_distance 最大查询距离（如果可用坐标信息）
 */
void generate_spatial_queries(const char* folder, Graph* g, int count, 
                             double max_distance);

/**
 * @brief 验证查询文件的有效性
 * @param folder 查询文件所在的文件夹路径
 * @param filename 查询文件名
 * @param g 图结构指针（用于验证节点索引）
 * @return 有效查询对数量，-1表示文件打开失败
 */
int validate_query_file(const char* folder, const char* filename, Graph* g);

/**
 * @brief 获取查询文件的统计信息
 * @param folder 查询文件所在的文件夹路径
 * @param filename 查询文件名
 * @param g 图结构指针
 */
void analyze_query_file(const char* folder, const char* filename, Graph* g);

#endif /* GENERATE_QUERIES_H */