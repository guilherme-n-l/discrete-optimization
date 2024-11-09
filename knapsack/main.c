#include <stdio.h>
#include <stdlib.h>

typedef struct item_t {
    unsigned long index;
    long value;
    unsigned long weight;
} item_t;

void getItems(unsigned long nItems, item_t *items) {
    for (long i = 0; i < nItems; i++) {
        item_t it;
        it.index = i;
        scanf("%ld %ld", &it.value, &it.weight);
        items[i] = it;
    }
}

char chooseBringing(unsigned long idx, unsigned long jdx, long *oldRow, item_t item) {
    long diagJdx = jdx - (1 + item.weight);

    if (idx != 0) {
        long upVal = oldRow[jdx];

        if (diagJdx >= -1) {
            return upVal < item.value + (diagJdx >= 0 ? oldRow[diagJdx] : 0);
        }

        return 0;
    }

    return diagJdx >= -1;
}

long updatedValue(char bring, unsigned long idx, unsigned long jdx, long *oldRow, item_t item) {
    long diagJdx = jdx - (1 + item.weight);

    if (idx != 0) {
        return bring ? (item.value + (diagJdx >= 0 ? oldRow[diagJdx] : 0)) : oldRow[jdx];
    }

    return bring ? item.value : 0;
}

void swapRows(long **pOldRow, long **pRow) {
    long *tmp = *pOldRow;
    *pOldRow = *pRow;
    *pRow = tmp;
}

void getIndices(unsigned long idx, unsigned long jdx, char **table, item_t *items, unsigned char *list) {
    for (long i = idx; i > 0; i--) {
        item_t it = items[i];
        if (table[i][jdx] != table[i - 1][jdx]) {
            list[i] = 1;
            jdx -= it.weight;
        } 
    }
    list[0] = table[0][jdx];
}

void printResults(unsigned long nItems, unsigned char *arr) {
    for (long i = 0; i < nItems; i++) {
        printf(i + 1 == nItems ? "%d" : "%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    unsigned long nItems;
    unsigned long cap;
    scanf("%ld %ld", &nItems, &cap);

    item_t *items = malloc(nItems * sizeof(item_t));
    getItems(nItems, items);

    char **table = malloc(nItems * sizeof(unsigned char*));

    long *oldRow = calloc(cap, sizeof(long)), *row = calloc(cap, sizeof(long));

    for (long i = 0; i < nItems; i++) {
        table[i] = malloc(cap * sizeof(unsigned char));
        item_t it = items[i];

        for (long j = 0; j < cap; j++) {
            char brought = chooseBringing(i, j, oldRow, it);

            table[i][j] = brought ? (i == 0 ? 1 : !table[i - 1][j]) : (i == 0 ? 0 : table[i - 1][j]);

            row[j] = updatedValue(brought, i, j, oldRow, it);
        }
        swapRows(&oldRow, &row);
    }

    printf("%ld 0\n", oldRow[cap - 1]);
    free(oldRow);
    free(row);

    unsigned char* str = calloc(nItems, sizeof(char));
    getIndices(nItems - 1, cap - 1, table, items, str);
    free(items);
    printResults(nItems, str);
    for (int i; i < nItems; i++) {
        free(table[i]);
    }
    free(table);
    free(str);

    return 0;
}
