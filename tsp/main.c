#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

int main() {
    (void)scanf("%d", &n);

    distances = malloc(DISTANCES_LEN * sizeof(double));
    points = malloc(n * sizeof(point_t));
    get_points();

    for (int i = 0; i < n; i++) print_point(i);
    
    free(distances);
    free(points);
    return 0;
}
