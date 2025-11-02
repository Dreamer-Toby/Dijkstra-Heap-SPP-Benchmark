#include <stdlib.h>
#include <stdio.h>
#include "pairingheap.h"

static void free_pair_node(PairNode *node);

static PairNode *pair_merge(PairNode *a, PairNode *b) {
    if (!a) return b;
    if (!b) return a;
    if (b->key < a->key) { 
        PairNode *t = a; 
        a = b; 
        b = t; 
    }
    b->sibling = a->child;
    if (b->sibling) b->sibling->prev = b; // 修复：添加prev指针维护
    a->child = b;
    b->parent = a;
    b->prev = NULL; // 修复：初始化prev指针
    return a;
}

static PairNode *pair_combine(PairNode *first) {
    if (!first) return NULL;

    // 使用动态数组存储合并后的树
    int cap = 128, n = 0;
    PairNode **arr = malloc(cap * sizeof(PairNode*));

    PairNode *current = first;
    while (current) {
        PairNode *a = current;
        PairNode *b = NULL;
        current = current->sibling;
        a->sibling = NULL;

        if (current) {
            b = current;
            current = current->sibling;
            b->sibling = NULL;
        }

        if (n >= cap) {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(PairNode*));
        }

        arr[n++] = pair_merge(a, b);
    }

    // 修复：从右向左合并
    while (n > 1) {
        int new_n = 0;
        for (int i = 0; i < n; i += 2) {
            if (i + 1 < n) {
                arr[new_n++] = pair_merge(arr[i], arr[i+1]);
            } else {
                arr[new_n++] = arr[i];
            }
        }
        n = new_n;
    }

    PairNode *result = (n > 0) ? arr[0] : NULL;
    free(arr);
    return result;
}

static void free_pair_node(PairNode *node) {
    if (!node) return;
    // 修复：避免递归释放兄弟节点导致的重复释放
    PairNode *child = node->child;
    PairNode *sibling = node->sibling;
    free(node);
    if (child) free_pair_node(child);
    if (sibling) free_pair_node(sibling);
}

PairingHeap *pair_create(int n) {
    PairingHeap *h = malloc(sizeof(PairingHeap));
    h->root = NULL;
    h->map = calloc(n, sizeof(PairNode *));
    h->n = n;
    return h;
}

void pair_insert(PairingHeap *h, double key, int val) {
    if (val < 0 || val >= h->n) return;
    if (h->map[val]) {
        pair_decrease_key(h, val, key);
        return;
    }

    PairNode *node = malloc(sizeof(PairNode));
    node->key = key;
    node->value = val;
    node->child = NULL;
    node->sibling = NULL;
    node->parent = NULL;
    node->prev = NULL; // 修复：初始化prev指针

    h->root = pair_merge(h->root, node);
    h->map[val] = node;
}

int pair_extract_min(PairingHeap *h) {
    if (!h->root) return -1;

    int v = h->root->value;
    h->map[v] = NULL;

    PairNode *old_root = h->root;
    PairNode *first_child = h->root->child;
    
    // 修复：断开子节点的父指针
    PairNode *child = first_child;
    while (child) {
        child->parent = NULL;
        child = child->sibling;
    }

    h->root = pair_combine(first_child);
    free(old_root);
    return v;
}

void pair_decrease_key(PairingHeap *h, int val, double newKey) {
    if (val < 0 || val >= h->n) return;
    PairNode *x = h->map[val];
    if (!x) {
        pair_insert(h, newKey, val);
        return;
    }

    if (newKey >= x->key) return;
    x->key = newKey;

    if (x == h->root) return;

    PairNode *p = x->parent;
    if (!p) return;

    // 修复：从父节点链表中正确移除节点
    if (p->child == x) {
        p->child = x->sibling;
        if (x->sibling) {
            x->sibling->prev = NULL;
        }
    } else {
        if (x->prev) {
            x->prev->sibling = x->sibling;
        }
        if (x->sibling) {
            x->sibling->prev = x->prev;
        }
    }

    x->parent = NULL;
    x->sibling = NULL;
    x->prev = NULL;
    h->root = pair_merge(h->root, x);
}

void pair_free(PairingHeap *h) {
    if (!h) return;
    if (h->root) free_pair_node(h->root);
    free(h->map);
    free(h);
}