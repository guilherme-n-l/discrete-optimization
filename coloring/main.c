#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define POPULATION_SIZE 150
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
void printIntArray(int n, int* arr) {
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("]\n");
}



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
            float score = scores[i] == 0 ? 0.1 : scores[i];
            sum += (float)1 / (score);
        }

        float partitions[POPULATION_SIZE];
        float collector2 = 0;
        int j = 0;
        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (iSelected == 1 && selected[0] == i) {
                partitions[j++] = collector2;
                continue;
            }
            float score = scores[i] == 0 ? 0.1 : scores[i];
            partitions[j++] = (((float)1 / score / sum)) + collector2;
            collector2 += (((float)1 / score) / sum);
        }

        if (iSelected == 1 && selected[0] == 0) {
            partitions[0] = -1;
        }

        float chosen = (float)rand()/RAND_MAX;
        int i = 0;
        while (chosen >= partitions[i]) {
            i++;
        };

        selected[iSelected] = i;
    }
}

void swapPositions(int idx, int jdx, int* arr) {
    int tmp = arr[idx];
    arr[idx] = arr[jdx];
    arr[jdx] = arr[tmp];
}

void removeDupesInArray(int n, int* arr) {
    int *freq = calloc(n, sizeof(int));
    int *idxs = malloc(n * sizeof(int));
    int *missing = malloc(n * sizeof(int));

    int nDuplicates = 0;
    for (int i = 0; i < n; i++) {
        if (freq[arr[i]] > 0) {
            idxs[nDuplicates++] = i;
        }

        freq[arr[i]]++;
    }

    if (nDuplicates == 0) {
        free(freq);
        free(idxs);
        free(missing);
        return;
    }

    int control = nDuplicates;
    nDuplicates = 0;
    for (int i = 0; i < n; i++) {
        if (freq[i] == 0) {
            missing[nDuplicates++] = i;
        }
    }

    if (control != nDuplicates) {
        printf("uai %d %d\n", control, nDuplicates);
        printIntArray(n, arr);
        printIntArray(n, freq);
    }

    for (int i = 0; i < nDuplicates; i++) {
        arr[idxs[i]] = missing[i];
    }

    free(freq);
    free(idxs);
    free(missing);
}

void crossover(int nNodes, int* p1, int* p2, int *offspring[2]) {

    for (int i = 0; i < 2; i++) {
        int splitIdx = rand() % nNodes;

        offspring[i] = malloc(nNodes * sizeof(int));

        char firstSlice = rand() % 2;
        
        memcpy(offspring[i], firstSlice ? p1 : p2, nNodes * sizeof(int));
        memcpy(offspring[i] + splitIdx, (firstSlice ? p2 : p1) + splitIdx, (nNodes - splitIdx) * sizeof(int));

        if ((float)rand() / RAND_MAX < MUTATION_RATE) {
            swapPositions(rand() % nNodes, rand() % nNodes, offspring[i]);
        }

        removeDupesInArray(nNodes, offspring[i]);
    }
}

void weakerSelection(int selected[2], int losers[2]) {
    int rangeLen = POPULATION_SIZE - 2;
    int *range = malloc(rangeLen * sizeof(int));

    rangeLen = 0;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        if (i == selected[0] || i == selected[1]) {
            continue;
        }
        range[rangeLen++] = i;
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
        if (array[i] < min) {
            min = array[i];
            idx = i;
        }
    }

    return idx;
}

int main() {
    srand(time(NULL) ^ getpid());
    int n, e;  
    scanf("%d %d", &n, &e);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    getInput(n, e, ht);

    int *population[POPULATION_SIZE];
    getPopulation(n, population);

    int *bestInput;

    for (int iGen = 0; iGen < N_GENERATIONS; iGen++) {
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
