#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "fibheap.h"
#include "pairingheap.h"

// Fibonacci heap Dijkstra
double* dijkstra_fibheap(const Graph *g, int s);

// Pairing heap Dijkstra  
double* dijkstra_pairingheap(const Graph *g, int s);

#endif