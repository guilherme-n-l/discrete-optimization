#include "pq.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define UP(i) ((i - 1) >> 1)
#define LEFT(i) (2 * i + 1)
#define RIGHT(i) (LEFT(i) + 1)

void sift_down(pq_t *pq, size_t i, char is_left) {
    size_t child_idx = is_left ? LEFT(i) : RIGHT(i);

    edge_t tmp = pq->arr[i];
    pq->arr[i] = pq->arr[child_idx];

    pq->arr[child_idx] = tmp;
}

void sift_up(pq_t *pq, size_t i) {
    edge_t tmp = pq->arr[i];
    pq->arr[i] = pq->arr[UP(i)];

    pq->arr[UP(i)] = tmp;
}

pq_t *pq_create(size_t size) {
    pq_t *pq_ptr = malloc(sizeof(pq_t));
    pq_ptr->arr = malloc(size * sizeof(edge_t));
    pq_ptr->size = size;
    pq_ptr->len = 0;

    return pq_ptr;
}

void pq_destroy(pq_t *pq) {
    free(pq->arr);
    free(pq);
}

char pq_is_empty(pq_t *pq) {
    return !pq->len;
}

void pq_insert(pq_t *pq, edge_t edge) {
    if (pq->len + 1 > pq->size) abort();

    size_t idx = pq->len;
    pq->arr[pq->len++] = edge;
    
    while (idx > 0 && pq->arr[UP(idx)].weight > edge.weight) {
        sift_up(pq, idx);
        idx = UP(idx);
    }
}

edge_t *pq_peek(pq_t *pq) {
    return &pq->arr[0];
}

edge_t pq_remove(pq_t *pq) {
    if (pq->len == 0) abort();

    edge_t min_edge = *pq_peek(pq);

    size_t idx = --pq->len;

    if (idx == 0) return min_edge;

    pq->arr[0] = pq->arr[idx];

    idx = 0;
    while (LEFT(idx) < pq->len) {
        double l = pq->arr[LEFT(idx)].weight, r = pq->arr[RIGHT(idx)].weight;
        double min_val = MIN(l, r);

        if (min_val >= pq->arr[idx].weight) break;

        char is_left = min_val == l;
        sift_down(pq, idx, is_left);
        idx = is_left ? LEFT(idx) : RIGHT(idx);
    }

    return min_edge;
}
