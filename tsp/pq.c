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

    int_double_t tmp = pq->arr[i];
    pq->arr[i] = pq->arr[child_idx];

    pq->arr[child_idx] = tmp;
}

void sift_up(pq_t *pq, size_t i) {
    int_double_t tmp = pq->arr[i];
    pq->arr[i] = pq->arr[UP(i)];

    pq->arr[UP(i)] = tmp;
}

pq_t *pq_create(size_t size) {
    pq_t *pq_ptr = malloc(sizeof(pq_t));
    pq_ptr->arr = malloc(size * sizeof(int_double_t));
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

void pq_insert(pq_t *pq, int_double_t id) {
    if (pq->len + 1 > pq->size) abort();

    size_t idx = pq->len;
    pq->arr[pq->len++] = id;
    
    while (idx > 0 && pq->arr[UP(idx)].value > id.value) {
        sift_up(pq, idx);
        idx = UP(idx);
    }
}

int_double_t *pq_peek(pq_t *pq) {
    return &pq->arr[0];
}

int_double_t pq_remove(pq_t *pq) {
    if (pq->len == 0) abort();

    size_t idx = --pq->len;

    if (idx == 0) return *pq_peek(pq);

    int_double_t return_id = *pq_peek(pq);
    pq->arr[0] = pq->arr[idx];
    int_double_t id = pq->arr[(idx = 0)];

    double left = pq->arr[LEFT(idx)].value, right = pq->arr[RIGHT(idx)].value;
    double min_child_val = MIN(left, right);
    char is_left = min_child_val == left;

    while ((LEFT(idx) < pq->len || RIGHT(idx) < pq->len) && min_child_val < id.value) {
        left = (LEFT(idx) < pq->len ? pq->arr[LEFT(idx)].value : DBL_MAX), right = (RIGHT(idx) < pq->len ? pq->arr[RIGHT(idx)].value : DBL_MAX);
        min_child_val = MIN(left, right);
        is_left = min_child_val == left;

        sift_down(pq, idx, is_left);
        idx = is_left ? LEFT(idx) : RIGHT(idx);
    }

    return return_id;
}
