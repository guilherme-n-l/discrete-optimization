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
    pq_t *pq = pq_create(n * n);
    pq_insert(pq, (edge_t){.source_id=0, .weight=0});

    char *visited = calloc(n, sizeof(char));

    double *key = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) key[i] = DBL_MAX;
    key[0] = 0;

    while (!pq_is_empty(pq)) {
        int source = pq_remove(pq).source_id;

        if (visited[source]) continue;

        visited[source] = 1;

        for (int i = 0; i < n; i++) {
            double dist = DISTANCE(source, i);
            if (i == source || visited[i] || key[i] <= dist) continue;

            key[i] = dist;
            tree[i - 1] = (edge_t){.source_id=source, .target_id=i, .weight=dist};
            pq_insert(pq, (edge_t){.source_id=i, .weight=key[i]});
        }
    }

    pq_destroy(pq);
    free(visited);
    free(key);
}

void get_deg(edge_t *edges, int *deg) {
    for (int i = 0; i < (n - 1); i++) {
        deg[edges[i].source_id]++;
        deg[edges[i].target_id]++;
    }
}

void perfect_matching(int *odds, int odds_len, edge_t *edges) {
    char *chosen = calloc(n, sizeof(char));

    for (int i = 0; i < odds_len - 1; i++) {
        if (chosen[i]) continue;

        double min_weight = DBL_MAX;
        edge_t *min_edge;

        for (int j = i + 1; j < odds_len; j++) {
            if (chosen[j]) continue;

            double weight = DISTANCE(odds[i], odds[j]);

            if (weight < min_weight) {
                min_weight = weight;
                min_edge = &(edge_t){.source_id=odds[i], .target_id=odds[j], .weight=min_weight};
            }
        }

        chosen[min_edge->source_id] = 1, chosen[min_edge->target_id] = 1;
    }

    free(chosen);
}


int main() {
    (void)scanf("%d", &n);

    distances = malloc(DISTANCES_LEN * sizeof(double));
    points = malloc(n * sizeof(point_t));
    get_points();

    // for (int i = 0; i < n; i++) print_point(i);

    edge_t *mst_edges = malloc((n - 1) * sizeof(edge_t));
    prim(mst_edges);
    
    int *deg = calloc(n, sizeof(int)), *odds = malloc(n * sizeof(int)), odds_len =  0;
    get_deg(mst_edges, deg);

    // for (int i = 0; i < (n - 1); i++) printf("%d -> %d w = %lf\n", mst_edges[i].source_id, mst_edges[i].target_id, mst_edges[i].weight);
    for (int i = 0; i < n; i++) if (deg[i] & 1) odds[odds_len++] = i;

    edge_t *multigraph = malloc((n - 1 + odds_len / 2) * sizeof(edge_t));
    perfect_matching(odds, odds_len, multigraph);

    free(multigraph);
    free(deg);
    free(distances);
    free(points);
    return 0;
}
