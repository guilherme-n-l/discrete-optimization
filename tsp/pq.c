#include "pq.h"
#include <stdlib.h>

void swap(pq_t *pq, size_t i, size_t j) {
    int_double_t tmp = pq->arr[i];
    pq->arr[i] = pq->arr[j];
    pq->arr[j] = tmp;
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
    if (pq_is_empty(pq)) {
        pq->arr[++pq->len] = id;
        return;
    }
}
