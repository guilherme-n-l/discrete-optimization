#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int nNeighs;
    unsigned int color;
    unsigned int* neighs;
} node_t;

char getNode(unsigned int idx, node_t **pNode, GHashTable* ht) {
    *pNode = (node_t *)g_hash_table_lookup(ht, GUINT_TO_POINTER(idx));

    char exists = *pNode != NULL;
    if (!exists) {
        node_t *node = g_new(node_t, 1);
        node->nNeighs = 0;

        g_hash_table_insert(ht, GUINT_TO_POINTER(idx), node);

        *pNode = node;
    };

    return exists;
}

void mallocNeighs(gpointer k, gpointer v, gpointer args) {
    node_t *node = (node_t *)v;
    node->neighs = malloc(node->nNeighs * sizeof(unsigned int));
}

void freeNeighs(gpointer k, gpointer v, gpointer args) {
    node_t *node = (node_t *)v;
    free(node->neighs);
}

void collectNeighs(unsigned int n, unsigned int e, unsigned int (*st)[2], GHashTable* ht) {
    unsigned int *idxNeighs = calloc(n, sizeof(unsigned int));

    for (int i = 0; i < e; i++) {
        unsigned int *pair = st[i];

        node_t *node;
        unsigned int nodeIdx = pair[0];
        getNode(nodeIdx, &node, ht);

        node->neighs[idxNeighs[nodeIdx]++] = pair[1];

        nodeIdx = pair[1];
        getNode(nodeIdx, &node, ht);
        node->neighs[idxNeighs[nodeIdx]++] = pair[0];
    }

    free(idxNeighs);
}

void testPrint(gpointer k, gpointer v, gpointer args) {
    node_t *node = (node_t *)v;
    unsigned int nNeighs = node->nNeighs;
    printf("%u - %u( ", GPOINTER_TO_UINT(k), nNeighs);
    for (int i = 0; i < nNeighs; i++) {
        printf("%u ", node->neighs[i]);
    }
    printf(")\n");
}

void getInput(unsigned int n, unsigned int e, GHashTable* ht) {
    unsigned int (*st)[2] = malloc(e * sizeof(*st));

    for (int i = 0; i < e; i++) {
        scanf("%u %u", &st[i][0], &st[i][1]);

        node_t *node;

        getNode(st[i][0], &node, ht);
        node->nNeighs++;

        getNode(st[i][1], &node, ht);
        node->nNeighs++;
    }

    for (int i = 0; i < e; i++) {
        printf("p1(%u) p2(%u)\n", st[i][0], st[i][1]);
    }

    g_hash_table_foreach(ht, mallocNeighs, NULL);
    collectNeighs(n, e, st, ht);

    free(st);
}

int main() {
    unsigned int n, e;  
    scanf("%u %u", &n, &e);
    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    getInput(n, e, ht);
    g_hash_table_foreach(ht, testPrint, NULL);
    g_hash_table_foreach(ht, freeNeighs, NULL);
    return 0;
}
