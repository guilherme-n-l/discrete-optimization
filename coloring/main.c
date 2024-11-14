#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POPULATION_SIZE 10
#define N_GENERATIONS 10
#define MUTATION_RATE 0.1

typedef struct {
    int id;
    int nNeighs;
    int color;
    int* neighs;
} node_t;

void getNode(int idx, node_t **ppNode, GHashTable* ht) {
    *ppNode = (node_t *)g_hash_table_lookup(ht, GUINT_TO_POINTER(idx));

    if (*ppNode == NULL) {
        *ppNode = g_new0(node_t, 1);
        (*ppNode)->id = idx;
        (*ppNode)->color = -1;

        g_hash_table_insert(ht, GUINT_TO_POINTER(idx), *ppNode);
    };
}

void mallocNeighs(gpointer k, gpointer v, gpointer args) {
    node_t *pNode= (node_t *)v;

    int nNeighs = pNode->nNeighs;

    if (nNeighs > 0) {
        pNode->neighs = malloc(nNeighs * sizeof(int));
        return;
    }

    pNode->neighs = NULL;
}

void freeNeighs(gpointer k, gpointer v, gpointer args) {
    int *neighs = ((node_t *)v)->neighs;
    if (neighs != NULL) {
        free(neighs);
    }
}

void freeNodes(gpointer k, gpointer v, gpointer args) {
    g_free(v);
}

void collectNeighs(int n, int e, int (*st)[2], GHashTable* ht) {
    int *idxNeighs = calloc(n, sizeof(int));

    for (int i = 0; i < e; i++) {
        node_t *pNode;
        for (int j = 0; j < 2; j++) {
            int nodeIdx = st[i][j];

            getNode(nodeIdx, &pNode, ht);
            pNode->neighs[idxNeighs[nodeIdx]++] = st[i][!j];
        }
    }

    free(idxNeighs);
}

// void printPNode(gpointer k, gpointer v, gpointer args) {
//     node_t *pNode = (node_t *)v;
//
//     printf("Node %d (color=%d) { ", pNode->id, pNode->color);
//     for (int i = 0; i < pNode->nNeighs; i++) {
//         printf("%d ", pNode->neighs[i]);
//     }
//     printf("} (len=%d)\n", pNode->nNeighs);
// }
//
// void printNode(node_t* pNode) {
//     printf("Node %d (color=%d) { ", pNode->id, pNode->color);
//     for (int i = 0; i < pNode->nNeighs; i++) {
//         printf("%d ", pNode->neighs[i]);
//     }
//     printf("} (len=%d)\n", pNode->nNeighs);
// }

void getInput(int n, int e, GHashTable* ht) {
    int (*st)[2] = malloc(e * sizeof(*st));

    for (int i = 0; i < e; i++) {
        scanf("%d %d", &st[i][0], &st[i][1]);

        node_t *node;
        for (int j = 0; j < 2; j++) {
            getNode(st[i][j], &node, ht);
            node->nNeighs++;
        }
    }

    g_hash_table_foreach(ht, mallocNeighs, NULL);
    collectNeighs(n, e, st, ht);

    free(st);
}

void getNeighborsColors(node_t *pNode, GHashTable *ht, int *colors) {
    for (int i = 0; i < pNode->nNeighs; i++) {
        node_t *pNeigh;
        getNode(pNode->neighs[i], &pNeigh, ht);

        colors[i] = pNeigh->color;
    }
}

void assignColors(int n, node_t **nodes , GHashTable *ht) {
    for (int i = 0; i < n; i++) {
        node_t *pNode = nodes[i];

        int nNeighs = pNode->nNeighs;
        int *neighColors = malloc(nNeighs * sizeof(int));

        getNeighborsColors(pNode, ht, neighColors);
        for (int j = 0; j < n; j++) {
            char dup = 0;
            for (int k = 0; k < nNeighs; k++) {
                dup |= j == neighColors[k];

                if (dup) {
                    break;
                }
            }

            if (!dup) {
                pNode->color = j;
                break;
            }
        }

        free(neighColors);
    }

}

int getNColors(int nNodes, node_t **nodes) {
    int nColors = INT_MIN;
    for (int i = 0; i < nNodes; i++) {
        nColors = MAX(nColors, nodes[i]->color);
    }

    return nColors + 1;
}

void getNodeArray(int nNodes, GHashTable *ht, node_t **nodes, int* order) {
    for (int i = 0; i < nNodes; i++) {
        getNode(order == NULL ? i : order[i], &nodes[i], ht);
    }
}

void cleanColor(gpointer k, gpointer v, gpointer args) {
    ((node_t *)v)->color = -1;
}

int greedy(int nNodes, GHashTable *ht, int *nodeOrder, int* colors) {
    node_t **nodes = malloc(nNodes * sizeof(node_t *));
    getNodeArray(nNodes, ht, nodes, nodeOrder);

    assignColors(nNodes, nodes, ht);

    if (colors != NULL) {
        for (int i = 0; i < nNodes; i++) {
            node_t *pNode;
            getNode(i, &pNode, ht);
            colors[i] = pNode->color;
        }
    }

    int nColors = getNColors(nNodes, nodes);

    free(nodes);

    g_hash_table_foreach(ht, cleanColor, NULL);
    return nColors;
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
    for (int iSelected = 0; iSelected < 2; iSelected++) {
        float sum = 0;
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (iSelected == 1 && selected[0] == i) {
                continue;
            }
            float score = scores[i] == 0 ? 0.5 : scores[i];
            sum += (float)1 / (score);
        }

        float partitions[POPULATION_SIZE];
        float collector2 = 0;
        int j = 0;
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (iSelected == 1 && selected[0] == i) {
                continue;
            }
            float score = scores[i] == 0 ? 0.5 : scores[i];
            partitions[j++] = (((float)1 / score / sum)) + collector2;
            collector2 += (((float)1 / score) / sum);
        }

        float chosen = (float)rand()/RAND_MAX;
        int i = 0;
        while (chosen >= partitions[i]) {
            i++;
        };

        selected[iSelected] = iSelected == 0 || (iSelected == 1 && i < selected[0]) ? i : i + 1;
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
        char firstSlice = rand() % 2;
        
        memcpy(offspring[i], firstSlice ? p1 : p2, nNodes * sizeof(int));
        memcpy(offspring[i] + splitIdx, (firstSlice ? p2 : p1) + splitIdx, (nNodes - splitIdx) * sizeof(int));

        if ((float)rand() / RAND_MAX < MUTATION_RATE) {
            swapPositions(rand() % nNodes, rand() % nNodes, offspring[i]);
        }

        int *freq = calloc(nNodes, sizeof(int));
        int nDuplicates = 0;

        for (int j = 0; j < nNodes; j++) {
            freq[offspring[i][j]]++;
            if (freq[offspring[i][j]] > 1) {
                nDuplicates++;
            }
        }

        while (nDuplicates > 0) {
            int aV = -1, bV = -1, dupIdx = -1;

            for (int j = 0; j < nNodes; j++) {
                if (freq[j] == 0) {
                    aV = j;
                } else if (freq[j] > 1) {
                    bV = j;
                }
            }

            for (int j = 0; j < nNodes; j++) {
                if (offspring[i][j] == bV) {
                    dupIdx = j;
                }
            }

            offspring[i][dupIdx] = aV;
            nDuplicates--;
        }

        free(freq);
    }
}

void weakerSelection(int selected[2], int losers[2]) {
    int rangeLen = POPULATION_SIZE - 2;
    int *range = malloc(rangeLen * sizeof(int));

    int i = 0, j = 0;
    while (j < rangeLen) {
        if (i != selected[0] && i != selected[1]) {
            range[j++] = i;
        }
        i++;
    }

    losers[0] = range[rand() % rangeLen];

    swapPositions(losers[0], rangeLen - 1, range);

    losers[1] = range[rand() % (rangeLen - 1)];

    free(range);
}

int arrayArgMin(int n, int array[]) {
    int min = INT_MAX;
    int idx = 0;

    for (int i = 0; i < n; i++) {
        min = MIN(min, array[i]);

        if (min == array[i]) {
            idx = i;
        }
    }

    return idx;
}

int main() {
    srand(time(NULL));
    int n, e;  
    scanf("%d %d", &n, &e);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    getInput(n, e, ht);

    int *population[POPULATION_SIZE];
    getPopulation(n, population);

    int *bestInput;

    for (int _ = 0; _ < N_GENERATIONS; _++) {
        int scores[POPULATION_SIZE];
        for (int i = 0; i < POPULATION_SIZE; i++) {
            scores[i] = greedy(n, ht, population[i], NULL);
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

        bestInput = population[arrayArgMin(POPULATION_SIZE, scores)];
    }

    int *colors = malloc(n * sizeof(int));
    printf("%d 0\n", greedy(n, ht, bestInput, colors));

    for (int i = 0; i < n; i++) {
        printf(i < n - 1 ? "%d " : "%d\n", colors[i]);
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        free(population[i]);
    }

    free(colors);
    g_hash_table_foreach(ht, freeNeighs, NULL);
    g_hash_table_foreach(ht, freeNodes, NULL);
    g_hash_table_destroy(ht);
    return 0;
}
