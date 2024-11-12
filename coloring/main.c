#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POPULATION_SIZE 5
#define N_GENERATIONS 5
#define MUTATION_RATE 0.1

typedef struct {
    int nNeighs;
    // int satDegree;
    int color;
    int* neighs;
} node_t;

char getNode(int idx, node_t **pNode, GHashTable* ht) {
    *pNode = (node_t *)g_hash_table_lookup(ht, GUINT_TO_POINTER(idx));

    char exists = *pNode != NULL;
    if (!exists) {
        node_t *node = g_new(node_t, 1);
        node->nNeighs = 0;
        node->color = -1;
        // node->satDegree = 0;

        g_hash_table_insert(ht, GUINT_TO_POINTER(idx), node);

        *pNode = node;
    };

    return exists;
}

void mallocNeighs(gpointer k, gpointer v, gpointer args) {
    node_t *node = (node_t *)v;
    node->neighs = malloc(node->nNeighs * sizeof(int));
}

void freeNeighs(gpointer k, gpointer v, gpointer args) {
    node_t *node = (node_t *)v;
    free(node->neighs);
}

void freeNodes(gpointer k, gpointer v, gpointer args) {
    g_free(v);
}

void collectNeighs(int n, int e, int (*st)[2], GHashTable* ht) {
    int *idxNeighs = calloc(n, sizeof(int));

    for (int i = 0; i < e; i++) {
        node_t *node;
        int nodeIdx = st[i][0];
        getNode(nodeIdx, &node, ht);

        node->neighs[idxNeighs[nodeIdx]++] = st[i][1];

        nodeIdx = st[i][1];
        getNode(nodeIdx, &node, ht);
        node->neighs[idxNeighs[nodeIdx]++] = st[i][0];
    }

    free(idxNeighs);
}

// void printPNode(gpointer k, gpointer v, gpointer args) {
//     node_t *pNode = (node_t *)v;
//
//     printf("Node %u (color=%d) { ", GPOINTER_TO_UINT(k), pNode->color);
//     for (int i = 0; i < pNode->nNeighs; i++) {
//         printf("%u ", pNode->neighs[i]);
//     }
//     printf("} (len=%u)\n", pNode->nNeighs);
// }

// void printNode(node_t* pNode) {
//     printf("Node (color=%d) { ", pNode->color);
//     for (int i = 0; i < pNode->nNeighs; i++) {
//         printf("%u ", pNode->neighs[i]);
//     }
//     printf("} (len=%u)\n", pNode->nNeighs);
// }

void getInput(int n, int e, GHashTable* ht) {
    int (*st)[2] = malloc(e * sizeof(*st));

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

// int compareHighestDeg(const void *a, const void *b) {
//     return -(((node_t *)a)->nNeighs - ((node_t *)b)->nNeighs);
// }

// int compareHighestSatDeg(const void *a, const void *b) {
//     int satA = ((node_t *)a)->satDegree;
//     int satB = ((node_t *)b)->satDegree;
//
//     if (satA == satB) {
//         return -(satA - satB);
//     }
//
//     return compareHighestDeg(a, b);
// }

void getNeighborColors(node_t *pNode, GHashTable *ht, int *colors) {
    for (int i = 0; i < pNode->nNeighs; i++) {
        node_t *pNeigh;
        getNode(pNode->neighs[i], &pNeigh, ht);

        colors[i] = pNeigh->color;
    }
}

void assignColor(int n, node_t *pNode, GHashTable *ht) {
    int nNeighs = pNode->nNeighs;
    int *neighColors = malloc(nNeighs * sizeof(int));

    getNeighborColors(pNode, ht, neighColors);

    for (int i = 0; i < n; i++) {
        unsigned char nodeWithColor = 0;

        for (int j = 0; j < nNeighs; j++) {
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

        // pNeigh->satDegree++;
    }

    free(neighColors);
}

int getNColors(int nNodes, node_t **nodes) {
    int nColors = 0;
    for (int i = 0; i < nNodes; i++) {
        nColors = MAX(nColors, nodes[i]->color);
    }
    return nColors + 1;
}

void getNodeArray(int nNodes, GHashTable *ht, node_t **nodes) {
    for (int i = 0; i < nNodes; i++) {
        node_t *pNode;
        getNode(i, &pNode, ht);

        nodes[i] = pNode;
    }
}

int greedy(int nNodes, GHashTable *ht, int *nodeOrder) {
    node_t **nodes = malloc(nNodes * sizeof(node_t *));
    getNodeArray(nNodes, ht, nodes);

    for (int i = 0; i < nNodes; i++) {
        node_t *pNode = nodes[nodeOrder[i]];
        assignColor(nNodes, pNode, ht);
    }

    int nColors = getNColors(nNodes, nodes);

    free(nodes);

    return nColors;
}

void cleanColor(gpointer k, gpointer v, gpointer args) {
    node_t *pNode = (node_t *)v;
    pNode->color=-1;
}

void shuffleArray(int n, int *arr) {
    if (n <= 1) {
        return;
    }
    for (int i = 0; i < n; i++) {
        int tmp, j = (i + rand()) % n;
        tmp = arr[j];
        arr[j] = arr[i];
        arr[i] = tmp;
    }
}

void getPopulation(int nNodes, int *arr[POPULATION_SIZE]) {
    int *range = malloc(nNodes * sizeof(int));

    for (int i = 0; i < nNodes; i++) {
        range[i] = i;
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        arr[i] = malloc(nNodes * sizeof(int));
        memcpy(arr[i], range, nNodes * sizeof(int));
        shuffleArray(nNodes, arr[i]);
    }

    free(range);
}

void partitionSelection(int *population[POPULATION_SIZE], int scores[POPULATION_SIZE], int selected[2]) {
    int collector = 0;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        collector += scores[i];
    }

    for (int i = 0; i < 2; i++) {
        int drained = rand() % collector + 1;

        int idx = 0;
        while (drained > 0) {
            drained -= scores[idx++];
        }

        selected[i] = idx;
    }
}

void swapPositions(int idx, int jdx, int* arr) {
    int tmp = arr[idx];
    arr[idx] = arr[jdx];
    arr[jdx] = arr[tmp];
}

void crossover(int nNodes, int* p1, int* p2, int *offspring[2]) {
    int splitIdx = rand() % nNodes;

    for (int i = 0; i < 2; i++) {
        offspring[i] = malloc(nNodes * sizeof(int));
        memcpy(offspring[i], p1, nNodes * sizeof(int));
        memcpy(offspring[i] + splitIdx, p2 + splitIdx, (nNodes - splitIdx) * sizeof(int));

        if ((float)rand() / RAND_MAX < MUTATION_RATE) {
            swapPositions(rand() % nNodes, rand() % nNodes, offspring[i]);
        }
    }
}

void weakerSelection(int selected[2], int losers[2]) {
    int rangeLen = POPULATION_SIZE - 2;
    int *range = malloc(rangeLen * sizeof(int));

    int i = 0, j = 0;
    while (i < POPULATION_SIZE) {
        if (i != selected[0] && i != selected[1]) {
            range[j++] = i;
        }
        i++;
    }

    for (int i = 0; i < 2; i++) {
        losers[i] = range[rand() % rangeLen];
    }

    free(range);
}

int main() {
    int n, e;  
    scanf("%u %u", &n, &e);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    getInput(n, e, ht);

    int *population[POPULATION_SIZE];
    getPopulation(n, population);

    for (int i = 0; i < N_GENERATIONS; i++) {
        int scores[POPULATION_SIZE];

        for (int j = 0; j < POPULATION_SIZE; j++) {
            scores[j] = greedy(n, ht, population[j]);
            g_hash_table_foreach(ht, cleanColor, NULL);
        }

        int selected[2];
        partitionSelection(population, scores, selected);

        int losers[2];
        weakerSelection(selected, losers);

        int *offspring[2];
        crossover(n, population[selected[0]], population[selected[1]], offspring);
        for (int i = 0; i < 2; i++) {
            free(population[losers[i]]);
            population[losers[i]] = offspring[i];
        }
    }


    for (int i = 0; i < n; i++) {
        node_t *pNode;
        getNode(i, &pNode, ht);
        printf(i < n - 1 ? "%u " : "%u\n", pNode->color);
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        free(population[i]);
    }
    g_hash_table_foreach(ht, freeNeighs, NULL);
    g_hash_table_foreach(ht, freeNodes, NULL);
    g_hash_table_destroy(ht);
    return 0;
}
