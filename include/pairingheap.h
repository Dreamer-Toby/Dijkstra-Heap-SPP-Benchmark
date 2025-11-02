// pairingheap.h
#ifndef PAIRINGHEAP_H
#define PAIRINGHEAP_H

typedef struct PairNode {
    double key;
    int value;
    struct PairNode *child;
    struct PairNode *sibling;
    struct PairNode *parent;
    struct PairNode *prev;  // 添加prev指针用于双向链表
} PairNode;

typedef struct {
    PairNode *root;
    PairNode **map;  // 值到节点的映射
    int n;           // 最大节点数
} PairingHeap;

PairingHeap *pair_create(int n);
void pair_insert(PairingHeap *h, double key, int val);
int pair_extract_min(PairingHeap *h);
void pair_decrease_key(PairingHeap *h, int val, double newKey);
void pair_free(PairingHeap *h);

#endif