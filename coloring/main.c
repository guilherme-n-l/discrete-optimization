#include <limits.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LOCAL_SEARCH_RANGE (int)1e3
#define TOLERANCE_INTERVAL 600

int *solution = NULL, *last_solution = NULL;
int solution_n = INT_MAX, last_solution_n = INT_MAX;
char stopped_by_time = 0;

typedef struct {
    int     id;
    int     n_neighs;
    int*    neighs;
} node_t;

void get_st(uint st_len, int (*st)[2]) {
    for (uint i = 0; i < st_len; i++) {
        scanf("%u %u", &st[i][0], & st[i][1]);
    }
}

void shuffle_int_array(int arr_len, int* arr) {
    for (int i = 0; i < arr_len; i++) {
        int new_idx = rand() % arr_len;
        int tmp = arr[new_idx];
        arr[new_idx] = arr[i];
        arr[i] = arr[new_idx];
    }
}

int *malloc_m1(size_t n, size_t __size) {
    int *arr = malloc(n * __size);

    for (size_t i = 0; i < n; i++) {
        arr[i] = -1;
    }

    return arr;
}

char is_neigh_color(node_t *node_ptr, GHashTable *ht, int color, int* colors) {
    char found_color = 0;

    for (int j = 0; j < node_ptr->n_neighs; j++) {
        found_color |= colors[node_ptr->neighs[j]] == color;
    }

    return found_color;
}

void greedy(int n, int *arr, GHashTable *ht) {
    int* colors = malloc_m1(n, sizeof(int));
    int max_color = INT_MIN;

    for (int i = 0; i < n; i++) {
        node_t *node_ptr = g_hash_table_lookup(ht, GINT_TO_POINTER(i));
        if (!node_ptr) {
            abort();
        }

        for (int j = 0; j < n; j++) {
            if (is_neigh_color(node_ptr, ht, j, colors)) {
                continue;
            }
            if (j > max_color) {
                max_color = j;
            }
            colors[i] = j;
        }
    }

    if (max_color < solution_n) {
        solution_n = max_color;
        last_solution_n = solution_n;
        solution = colors;
        last_solution = solution;
        return;
    }

    free(colors);
}

void insert_nodes(int n_nodes, uint st_len, GHashTable* ht, int (*st)[2]) {
    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = g_malloc(sizeof(node_t));
        node_ptr->id = i;

        node_ptr->n_neighs = 0;
        for (uint j = 0; j < st_len; j++) {
            if (st[j][0] == i) {
                node_ptr->n_neighs++;
            } else if (st[j][1] == i) {
                node_ptr->n_neighs++;
            }
        }

        g_hash_table_insert(ht, GINT_TO_POINTER(i), node_ptr);
    }

    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = g_hash_table_lookup(ht, GINT_TO_POINTER(i));
        node_ptr->neighs = malloc(node_ptr->n_neighs * sizeof(int));

        if (node_ptr) {
            int n_neighs = 0;
            for (int j = 0; j < st_len; j++) {
                if (st[j][0] == i) {
                    node_ptr->neighs[n_neighs++] = st[j][1];

                } else if (st[j][1] == i) {
                    node_ptr->neighs[n_neighs++] = st[j][0];
                }

                if (n_neighs == node_ptr->n_neighs) {
                    break;
                }
            }
        }
    }
}

void free_nodes(int n_nodes, GHashTable *ht) {
    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = g_hash_table_lookup(ht, GINT_TO_POINTER(i));
        if (node_ptr) {
            if (node_ptr->neighs) {
                free(node_ptr->neighs);
            }
            g_free(node_ptr);
        }
    }
}

char exceeded_time(time_t ss) {
    return difftime(time(NULL), ss) > TOLERANCE_INTERVAL;
}

void recursive(int n_colors, int n_nodes, int depth, int* arr, GHashTable* ht, time_t ss) {
    if (depth == n_nodes) {
        solution_n = n_colors;
        solution = arr;
        return;
    }

    node_t *node_ptr = g_hash_table_lookup(ht, GINT_TO_POINTER(depth));
    if (!node_ptr) {
        abort();
    }

    if (depth == 0) {
        arr = malloc_m1(n_nodes, sizeof(int));
        arr[0] = 0;
        recursive(n_colors, n_nodes, depth + 1, arr, ht, ss);
        if (!solution) {
            free(arr);
        }
        return;
    }

    for (int i = 0; i < n_colors; i++) {
        if (is_neigh_color(node_ptr, ht, i, arr)) {
            continue;
        }

        if ((stopped_by_time |= exceeded_time(ss))) {
            break;
        }

        arr[depth] = i;
        recursive(n_colors, n_nodes, depth + 1, arr, ht, ss);

        if (solution) {
            return;
        }

        arr[depth + 1] = -1;
    }
}

void print_int_array(int n, int* arr) {
    for (int i = 0; i < n; i++) {
        printf(i == (n - 1) ? "%d\n" : "%d ", arr[i]);
    }
}

int main() {
    srand(time(NULL));
    int n;
    uint m;
    scanf("%d %u", &n, &m);

    int (*st)[2] = malloc(m * sizeof(*st));
    get_st(m, st);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    insert_nodes(n, m, ht, st);

    free(st);

    for (int i = 0; i < LOCAL_SEARCH_RANGE; i++) {
        int *sample = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            sample[j] = j;
        }

        shuffle_int_array(n, sample);
        greedy(n, sample, ht);
    }

    time_t ss = time(NULL);

    while (solution && !(stopped_by_time |= exceeded_time(ss))) {
        solution = NULL;
        recursive(--solution_n, n, 0, NULL, ht, ss);

        if (solution) {
            last_solution = solution;
            last_solution_n = solution_n;
        }
    }
    free(solution);

    printf(stopped_by_time ? "%d 0\n" : "%d 1\n", last_solution_n);
    print_int_array(n, last_solution);
    free(last_solution);

    free_nodes(n, ht);
    g_hash_table_destroy(ht);
    return 0;
}
