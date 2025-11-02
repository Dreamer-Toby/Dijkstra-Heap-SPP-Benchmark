#ifndef FIBHEAP_H
#define FIBHEAP_H

#include <stdbool.h>

typedef struct FibHeap FibHeap;

// Public API
FibHeap* fib_create(int max_nodes);
void fib_insert(FibHeap *H, double key, int node);
int fib_extract_min(FibHeap *H);
void fib_decrease_key(FibHeap *H, int node, double new_key);
void fib_free(FibHeap *H);
bool fib_is_empty(FibHeap *H);  // 确保这个声明存在

#endif