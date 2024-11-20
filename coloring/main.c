#include <limits.h>
#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>

int* solution = NULL;
int solution_n = 1;

typedef struct {
    int     id;
    int     n_neighs;
    int*   neighs;
} node_t;

void get_st(uint st_len, int (*st)[2]) {
    for (uint i = 0; i < st_len; i++) {
        scanf("%u %u", &st[i][0], & st[i][1]);
    }
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

char is_neigh_color(node_t *node_ptr, GHashTable *ht, int color, int* colors) {
    char found_color = 0;

    for (int j = 0; j < node_ptr->n_neighs; j++) {
        found_color |= colors[node_ptr->neighs[j]] == color;
    }

    return found_color;
}

int *malloc_m1(size_t n, size_t __size) {
    int *arr = malloc(n * __size);

    for (size_t i = 0; i < n; i++) {
        arr[i] = -1;
    }

    return arr;
}

void recursive(int n_colors, int n_nodes, int depth, int* arr, GHashTable* ht) {
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
        recursive(n_colors, n_nodes, depth + 1, arr, ht);
        if (!solution) {
            free(arr);
        }
        return;
    }

    for (int i = 0; i < n_colors; i++) {
        if (is_neigh_color(node_ptr, ht, i, arr)) {
            continue;
        }

        arr[depth] = i;
        recursive(n_colors, n_nodes, depth + 1, arr, ht);

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

char has_edge(int n_nodes, GHashTable *ht) {
    char edge_found = 0;
    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = g_hash_table_lookup(ht, GINT_TO_POINTER(i));
        if (!node_ptr) {
            continue;
        }
        edge_found |= node_ptr->n_neighs > 0;

        if (edge_found) {
            break;
        }
    }
    return edge_found;
}

int main() {
    int n;
    uint m;
    scanf("%d %u", &n, &m);

    int (*st)[2] = malloc(m * sizeof(*st));
    get_st(m, st);

    GHashTable *ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    insert_nodes(n, m, ht, st);

    if (has_edge(n, ht)) {
        solution_n = 2;
    }

    while (!solution) {
        recursive(solution_n++, n, 0, NULL, ht);
    }

    printf("%d 0\n", solution_n);
    print_int_array(n, solution);

    free(solution);
    free_nodes(n, ht);
    g_hash_table_destroy(ht);
    free(st);
    return 0;
}
