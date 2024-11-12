#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int nNeighs;
    unsigned int satDegree;
    unsigned int color;
    unsigned int* neighs;
} node_t;

char getNode(unsigned int idx, node_t **pNode, GHashTable* ht) {
    *pNode = (node_t *)g_hash_table_lookup(ht, GUINT_TO_POINTER(idx));

    char exists = *pNode != NULL;
    if (!exists) {
        node_t *node = g_new(node_t, 1);
        node->nNeighs = 0;
        node->color = -1;
        node->satDegree = 0;

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
        node_t *node;
        unsigned int nodeIdx = st[i][0];
        getNode(nodeIdx, &node, ht);

        node->neighs[idxNeighs[nodeIdx]++] = st[i][1];

        nodeIdx = st[i][1];
        getNode(nodeIdx, &node, ht);
        node->neighs[idxNeighs[nodeIdx]++] = st[i][0];
    }

    free(idxNeighs);
}

void printPNode(gpointer k, gpointer v, gpointer args) {
    node_t *pNode = (node_t *)v;

    printf("Node %u (color=%d) { ", GPOINTER_TO_UINT(k), pNode->color);
    for (int i = 0; i < pNode->nNeighs; i++) {
        printf("%u ", pNode->neighs[i]);
    }
    printf("} (len=%u)\n", pNode->nNeighs);
}

void printNode(node_t* pNode) {
    printf("Node (color=%d) { ", pNode->color);
    for (int i = 0; i < pNode->nNeighs; i++) {
        printf("%u ", pNode->neighs[i]);
    }
    printf("} (len=%u)\n", pNode->nNeighs);
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

    g_hash_table_foreach(ht, mallocNeighs, NULL);
    collectNeighs(n, e, st, ht);

    free(st);
}

int compareHighestDeg(const void *a, const void *b) {
    return -(((node_t *)a)->nNeighs - ((node_t *)b)->nNeighs);
}

int compareHighestSatDeg(const void *a, const void *b) {

    unsigned int satA = ((node_t *)a)->satDegree;
    unsigned int satB = ((node_t *)b)->satDegree;

    if (satA == satB) {
        return -(satA - satB);
    }

    return compareHighestDeg(a, b);
}

void getNeighborColors(node_t *pNode, GHashTable *ht, unsigned int *colors) {
    for (int i = 0; i < pNode->nNeighs; i++) {
        node_t *pNeigh;
        getNode(pNode->neighs[i], &pNeigh, ht);

        colors[i] = pNeigh->color;
    }
}

void assignColor(unsigned int n, node_t *pNode, GHashTable *ht) {
    unsigned int nNeighs = pNode->nNeighs;
    unsigned int *neighColors = malloc(nNeighs * sizeof(unsigned int));

    getNeighborColors(pNode, ht, neighColors);

    for (unsigned int i = 0; i < n; i++) {
        unsigned char nodeWithColor = 0;

        for (unsigned int j = 0; j < nNeighs; j++) {
            node_t *pNeigh;
            getNode(pNode->neighs[j], &pNeigh, ht);

            nodeWithColor |= pNeigh->color == i;

            if (nodeWithColor) {
                break;
            }
        }

        if (!nodeWithColor) {
            pNode->color = i;
            break;
        }
    }

    for (int i = 0; i < nNeighs; i++) {
        node_t *pNeigh;
        getNode(i, &pNeigh, ht);

        pNeigh->satDegree++;
    }

    free(neighColors);
}

unsigned int getNColors(int nNodes, node_t **nodes) {
    unsigned int nColors = 0;
    for (int i = 0; i < nNodes; i++) {
        nColors = MAX(nColors, nodes[i]->color);
    }
    return nColors + 1;
}

int main() {
    unsigned int n, e;  
    scanf("%u %u", &n, &e);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    getInput(n, e, ht);

    node_t **nodes = malloc(n * sizeof(node_t *));

    for (int i = 0; i < n; i++) {
        node_t *pNode;
        getNode(i, &pNode, ht);

        nodes[i] = pNode;
    }

    qsort(nodes, n, sizeof(node_t *), compareHighestDeg);

    for (int i = 0; i < n; i++) {
        node_t *pNode = nodes[i];
        assignColor(n, pNode, ht);

        if (i < n - 1) {
            qsort(nodes + i + 1, n - (i + 1), sizeof(node_t *), compareHighestSatDeg);
        }
    }

    printf("%u 0\n", getNColors(n, nodes));
    for (int i = 0; i < n; i++) {
        node_t *pNode;
        getNode(i, &pNode, ht);
        printf(i < n - 1 ? "%u " : "%u\n", pNode->color);
    }

    free(nodes);
    g_hash_table_foreach(ht, freeNeighs, NULL);
    return 0;
}
