#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

int main() {
    pq_t *pq = pq_create(3);
    for (int i = 0; i < 3; i++) pq_insert(pq, (int_double_t){.idx=i , .value=random()});
    for (int i = 0; i < 3; i++)  {
        int_double_t id = pq_remove(pq);
        printf("%d %lf\n", id.idx, id.value);
    }
    pq_destroy(pq);
    return 0;
}
