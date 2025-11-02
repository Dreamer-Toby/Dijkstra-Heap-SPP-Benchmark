#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>
#include "fibheap.h"

// Fibonacci Heap Node
typedef struct FibNode {
    int node;           // Node index
    double key;         // Key value (distance)
    int degree;         // Number of children
    bool mark;          // Mark flag
    struct FibNode *parent;    // Parent pointer
    struct FibNode *child;     // Child pointer
    struct FibNode *left;      // Left sibling
    struct FibNode *right;     // Right sibling
} FibNode;

// Fibonacci Heap
struct FibHeap {
    FibNode *min;       // Pointer to minimum node
    int n;              // Number of nodes in heap
    int max_nodes;      // Maximum number of nodes
    FibNode **map;      // map[node_id] = FibNode*
};

// Create a new Fibonacci heap
FibHeap* fib_create(int max_nodes) {
    FibHeap *H = (FibHeap*)malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    H->max_nodes = max_nodes;
    H->map = (FibNode**)calloc(max_nodes, sizeof(FibNode*));
    return H;
}

// Create a new node
static FibNode* fib_new_node(int node, double key) {
    FibNode *x = (FibNode*)malloc(sizeof(FibNode));
    x->node = node;
    x->key = key;
    x->degree = 0;
    x->mark = false;
    x->parent = NULL;
    x->child = NULL;
    x->left = x;
    x->right = x;
    return x;
}

// Insert a new node into the heap
void fib_insert(FibHeap *H, double key, int node) {
    if (node < 0 || node >= H->max_nodes) {
        printf("Error: node index %d out of bounds (max: %d)\n", node, H->max_nodes);
        return;
    }
    
    // If node already exists, use decrease_key instead
    if (H->map[node] != NULL) {
        fib_decrease_key(H, node, key);
        return;
    }
    
    FibNode *x = fib_new_node(node, key);
    H->map[node] = x;
    
    if (H->min == NULL) {
        H->min = x;
    } else {
        // Insert into root list
        x->left = H->min;
        x->right = H->min->right;
        H->min->right->left = x;
        H->min->right = x;
        
        // Update min if needed
        if (key < H->min->key) {
            H->min = x;
        }
    }
    H->n++;
}

// Link node y to node x (make y a child of x)
static void fib_link(FibHeap *H, FibNode *y, FibNode *x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;
    
    // Make y a child of x
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->left = y->right = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = false;
}

// Consolidate the heap after extract_min
static void fib_consolidate(FibHeap *H) {
    if (H->min == NULL) return;
    
    int max_degree = (int)(log(H->n) / log(2)) + 2;
    FibNode **A = (FibNode**)calloc(max_degree + 1, sizeof(FibNode*));
    
    // Process all nodes in root list
    FibNode *w = H->min;
    int root_count = 0;
    
    // First, count root nodes and ensure we don't loop infinitely
    FibNode *current = w;
    do {
        root_count++;
        current = current->right;
    } while (current != w);
    
    // Process each root node
    for (int i = 0; i < root_count; i++) {
        FibNode *x = w;
        w = w->right;
        
        int d = x->degree;
        while (A[d] != NULL) {
            FibNode *y = A[d];
            if (x->key > y->key) {
                FibNode *temp = x;
                x = y;
                y = temp;
            }
            fib_link(H, y, x);
            A[d] = NULL;
            d++;
        }
        A[d] = x;
    }
    
    // Rebuild root list and find new min
    H->min = NULL;
    for (int i = 0; i <= max_degree; i++) {
        if (A[i] != NULL) {
            if (H->min == NULL) {
                H->min = A[i];
                A[i]->left = A[i]->right = A[i];
            } else {
                // Add to root list
                A[i]->left = H->min;
                A[i]->right = H->min->right;
                H->min->right->left = A[i];
                H->min->right = A[i];
                
                // Update min if needed
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
    
    free(A);
}

// Extract the minimum node
int fib_extract_min(FibHeap *H) {
    FibNode *z = H->min;
    if (z == NULL) return -1;
    
    // Add all children of z to root list
    if (z->child != NULL) {
        FibNode *child = z->child;
        FibNode *next_child;
        do {
            next_child = child->right;
            
            // Add child to root list
            child->left = H->min;
            child->right = H->min->right;
            H->min->right->left = child;
            H->min->right = child;
            
            child->parent = NULL;
            child = next_child;
        } while (child != z->child);
        z->child = NULL;
    }
    
    // Remove z from root list
    if (z->right == z) {
        // Only one node in root list
        H->min = NULL;
    } else {
        z->left->right = z->right;
        z->right->left = z->left;
        H->min = z->right;  // Temporarily set min to next node
        fib_consolidate(H);
    }
    
    int result = z->node;
    H->map[result] = NULL;
    free(z);
    H->n--;
    
    return result;
}

// Cut node x from its parent y and add x to root list
static void fib_cut(FibHeap *H, FibNode *x, FibNode *y) {
    // Remove x from y's child list
    if (x->right == x) {
        y->child = NULL;
    } else {
        if (y->child == x) {
            y->child = x->right;
        }
        x->left->right = x->right;
        x->right->left = x->left;
    }
    y->degree--;
    
    // Add x to root list
    x->left = H->min;
    x->right = H->min->right;
    H->min->right->left = x;
    H->min->right = x;
    
    x->parent = NULL;
    x->mark = false;
    
    // Update min if needed
    if (x->key < H->min->key) {
        H->min = x;
    }
}

// Cascading cut - recursively cut marked parents
static void fib_cascading_cut(FibHeap *H, FibNode *y) {
    FibNode *z = y->parent;
    if (z != NULL) {
        if (!y->mark) {
            y->mark = true;
        } else {
            fib_cut(H, y, z);
            fib_cascading_cut(H, z);
        }
    }
}

// Decrease key of a node
void fib_decrease_key(FibHeap *H, int node, double new_key) {
    if (node < 0 || node >= H->max_nodes) {
        printf("Error: node index %d out of bounds (max: %d)\n", node, H->max_nodes);
        return;
    }
    
    FibNode *x = H->map[node];
    if (x == NULL) {
        // Node not in heap, insert it
        fib_insert(H, new_key, node);
        return;
    }
    
    if (new_key > x->key) {
        // This should not happen in Dijkstra, but if it does, just return
        return;
    }
    
    x->key = new_key;
    FibNode *y = x->parent;
    
    if (y != NULL && x->key < y->key) {
        fib_cut(H, x, y);
        fib_cascading_cut(H, y);
    }
    
    // 确保 H->min 不为空
    if (H->min != NULL && x->key < H->min->key) {
        H->min = x;
    }
}

// Check if heap is empty
bool fib_is_empty(FibHeap *H) {
    return H->min == NULL;
}

// Recursively free a node and its children
static void fib_free_node(FibNode *node) {
    if (node == NULL) return;
    
    FibNode *current = node;
    FibNode *start = node;
    do {
        FibNode *next = current->right;
        if (current->child != NULL) {
            fib_free_node(current->child);
        }
        free(current);
        current = next;
    } while (current != start);
}

// Free the entire heap
void fib_free(FibHeap *H) {
    if (H == NULL) return;
    
    if (H->min != NULL) {
        fib_free_node(H->min);
    }
    
    free(H->map);
    free(H);
}