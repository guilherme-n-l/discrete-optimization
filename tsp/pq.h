#ifndef PQ_H
#define PQ_H
#include <stdlib.h>

typedef struct {
    int     source_id;
    int     target_id;
    double  weight;
} edge_t;

typedef struct {
    size_t          len;
    size_t          size;
    edge_t          *arr;
} pq_t;

pq_t *pq_create(size_t);
void pq_destroy(pq_t *);
void pq_insert(pq_t *, edge_t);
char pq_is_empty(pq_t *);
edge_t pq_remove(pq_t *);
edge_t *pq_peek(pq_t *);
#endif
