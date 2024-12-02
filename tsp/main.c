#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include "pq.h"

#define SQUARE(A) pow(A, 2)
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define CALC_DISTANCE(A, B) sqrt(SQUARE(points[A][0] - points[B][0]) + SQUARE(points[A][1] - points[B][1]))
#define DISTANCE_IDX(A, B) ((MAX(A, B) - 1) * MAX(A, B) / 2 + MIN(A, B))
#define DISTANCE(A, B) A == B ? .0 : (distances[DISTANCE_IDX(A, B)] != .0 ? distances[DISTANCE_IDX(A, B)] : (distances[DISTANCE_IDX(A, B)] = CALC_DISTANCE(A, B)))
#define DISTANCES_LEN (n * (n - 1) / 2)

typedef double point_t[2];

int n;
point_t *points;
double *distances;

void print_double_array(size_t n, double *arr) {
    printf("[ ");
    for (size_t i = 0; i < n; i++) printf("%lf ", arr[i]);
    printf("]\n");
}

void get_points() {
    for (int i = 0; i < n; i++) {
        (void)scanf("%lf %lf", &points[i][0], &points[i][1]);
    }
}

void print_point(int idx) {
    printf("Point: coords (%.4lf, %.4lf) distances [ ", points[idx][0], points[idx][1]);
    for (int i = 0; i < n; i++) printf("%lf ", DISTANCE(idx, i));
    printf("]\n");
}

void prim(edge_t *tree) {
    pq_t *pq = pq_create(n);
    pq_insert(pq, (edge_t){.source_id=0, .weight=0});

    char *visited = calloc(n, sizeof(char));

    double *key = malloc(n * sizeof(double));
    memset(key, DBL_MAX, n * sizeof(double));
    key[0] = 0;

    while (!pq_is_empty(pq)) {
        int source = pq_remove(pq).source_id;

        if (visited[source]) continue;

        visited[source] = 1;

        for (int i = 0; i < n; i++) {
            double dist = DISTANCE(source, i);
            if (i == source || visited[i] || key[i] <= dist) continue;

            key[i] = dist;
            tree[i - 1] = (edge_t){.source_id=source, .target_id=i, .weight=DISTANCE(source, i)};
            pq_insert(pq, (edge_t){.source_id=i, .weight=key[i]});
        }
    }
    pq_destroy(pq);
    free(visited);
    free(key);
}

int main() {
    (void)scanf("%d", &n);

    distances = malloc(DISTANCES_LEN * sizeof(double));
    points = malloc(n * sizeof(point_t));
    get_points();

    for (int i = 0; i < n; i++) print_point(i);

    edge_t *mst_edges = malloc((n - 1) * sizeof(edge_t));
    prim(mst_edges);

    // for (int i = 0; i < (n - 1); i++) printf("%d -> %d\n", mst_edges[i].source_id, mst_edges[i].target_id);
    
    int *deg = malloc(n * sizeof(int));
    for (int i = 0; i < (n - 1); i++) {
        deg[mst_edges[i].source_id]++;
        deg[mst_edges[i].target_id]++;
    }

    free(deg);
    free(distances);
    free(points);
    return 0;
}
