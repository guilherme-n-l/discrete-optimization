#ifndef PQ_H
#define PQ_H
#include <stdlib.h>

typedef struct {
    int     idx;
    double  value;
} int_double_t;

typedef struct {
    size_t          len;
    size_t          size;
    int_double_t    *arr;
} pq_t;

pq_t *pq_create(size_t);
void pq_destroy(pq_t *);
void pq_insert(pq_t *, int_double_t);
char pq_is_empty(pq_t *);
int_double_t pq_remove(pq_t *);
int_double_t *pq_peek(pq_t *);
#endif
