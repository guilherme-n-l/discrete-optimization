#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define square(A) pow(A, 2)
#define distance(A, B) sqrt(square(A[0] - B[0]) + square(A[1] - B[1]))

typedef struct {
    double  coords[2];
    int     distances_len;
    double* distances;
} point_t;

void get_coords(int n, point_t *points) {
    for (int i = 0; i < n; i++) {
        points[i] = (point_t){};
        (void)scanf("%lf %lf", &points[i].coords[0], &points[i].coords[1]);
    }
}

void get_distances(int n, point_t *points) {
    for (int i = 0; i < n; i++) {
        point_t *p_ptr = &points[i];

        p_ptr->distances_len = n;
        p_ptr->distances = malloc(p_ptr->distances_len * sizeof(double));

        for (int j = 0; j < p_ptr->distances_len; j++) {
            p_ptr->distances[j] = -1;
        }
    }

    for (int i = 0; i < n; i++) {
        point_t *p_ptr = &points[i];

        for (int j = 0; j < p_ptr->distances_len; j++) {
            point_t *p1_ptr = &points[j];

            if (j == i) {
                p_ptr->distances[j] = 0;
            }

            double neigh_distance = p1_ptr->distances[i];
            p_ptr->distances[j] = neigh_distance != -1 ? neigh_distance : distance(p_ptr->coords, p1_ptr->coords);
        }
    }
}

void get_points(int n, point_t *points) {
    get_coords(n, points);
    get_distances(n, points);
}

void print_point(point_t p) {
    printf("Point: coords (%.4lf, %.4lf), distances [ ", p.coords[0], p.coords[1]);

    for (int i = 0; i < p.distances_len; i++) {
        printf("%.4lf ", p.distances[i]);
    }

    printf("]\n");
}

void free_points(int n, point_t* points) {
    for (int i = 0; i < n; i++) {
        free(points[i].distances);
    }

    free(points);
}

int main() {
    int n;
    (void)scanf("%d", &n);

    point_t *points = malloc(n * sizeof(point_t));
    get_points(n, points);

    for (int i = 0; i < n; i++) {
        print_point(points[i]);
        printf("\n");
    }
    
    free_points(n, points);
    return 0;
}
