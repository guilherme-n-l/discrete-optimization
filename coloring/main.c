#include <glib-2.0/glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOCAL_SEARCH_RANGE  (int)1e4
#define N_GENERATIONS       6
#define MUTATION_RATE       .3
#define TOLERANCE_INTERVAL  1800
#define SHOULD_MUTATE (((float)rand() / RAND_MAX) < MUTATION_RATE)

#define SATUR_DEGREE(A)     satur[A]
#define HT_GET(A)           ((node_t *)g_hash_table_lookup(ht, GINT_TO_POINTER(A)))
#define HT_ADD(A, B)        g_hash_table_insert(ht, GINT_TO_POINTER(A), B)

GHashTable *ht;
int solution_n = INT_MAX, last_solution_n = INT_MAX;
int *solution = NULL, *last_solution = NULL, *satur;
char stopped_by_time = 0;

typedef struct {
    int     id;
    int     n_neighs;
    int     *neighs;
} node_t;

void print_char_array(int n, char* arr) {
    for (int i = 0; i < n; i++) printf(i == (n - 1) ? "%d\n" : "%d ", arr[i]);
}

void print_int_array(int n, int* arr) {
    for (int i = 0; i < n; i++) printf(i == (n - 1) ? "%d\n" : "%d ", arr[i]);
}

void print_double_array(int n, double* arr) {
    for (int i = 0; i < n; i++) printf(i == (n - 1) ? "%.4lf ]\n" : i == 0 ? "[ %.4lf " :  "%.4lf ", arr[i]);
}

void get_st(uint st_len, int (*st)[2]) {
    for (uint i = 0; i < st_len; i++) scanf("%u %u", &st[i][0], & st[i][1]);
}

void swap(int idx, int jdx, int* arr) {
    int tmp = arr[idx];
    arr[idx] = arr[jdx];
    arr[jdx] = tmp;
}

void shuffle_int_array(int arr_len, int* arr) {
    for (int i = 0; i < arr_len; i++) {
        int new_idx = rand() % arr_len;
        swap(i, new_idx, arr);
    }
}

int *malloc_m1(size_t n, size_t __size) {
    int *arr = malloc(n * __size);

    for (size_t i = 0; i < n; i++) arr[i] = -1;

    return arr;
}

char is_neigh_color(node_t *node_ptr, int color, int* colors) {
    char found_color = 0;

    for (int j = 0; j < node_ptr->n_neighs; j++) found_color |= colors[node_ptr->neighs[j]] == color;

    return found_color;
}

int greedy(int n, int *arr) {
    int* colors = malloc_m1(n, sizeof(int)), max_color = INT_MIN;

    for (int i = 0; i < n; i++) {
        node_t *node_ptr = HT_GET(arr[i]);

        for (int j = 0; j < n; j++) {
            if (is_neigh_color(node_ptr, j, colors)) continue;

            max_color = MAX(max_color, j);

            colors[arr[i]] = j;
            break;
        }
    }

    if (++max_color < solution_n) {
        last_solution_n = solution_n;
        solution_n = max_color;

        if (last_solution) free(last_solution);

        last_solution = solution;
        solution = colors;
    } else {
        free(colors);
    }

    return max_color;
}

int get_n_neighs(int node_idx, uint st_len, int (*st)[2]) {
    int collector = 0;

    for (uint j = 0; j < st_len; j++) {
        if (st[j][0] == node_idx || st[j][1] == node_idx) collector++;
    }

    return collector;
}

void get_neighs(int node_idx, uint st_len, int neighs_len, int (*st)[2], int* neighs) {
    int n_neighs = 0;

    for (uint i = 0; i < st_len; i++) {
        char node_jdx;
        if (!(node_jdx = (!(st[i][0] == node_idx))) || (node_jdx = (st[i][1] == node_idx))) neighs[n_neighs++] = st[i][!node_jdx];

        if (n_neighs == neighs_len) break;
    }
}

void insert_nodes(int n_nodes, uint st_len, int (*st)[2]) {
    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = g_malloc(sizeof(node_t));
        node_ptr->id = i;

        node_ptr->n_neighs = get_n_neighs(i, st_len, st);

        node_ptr->neighs = malloc(node_ptr->n_neighs * sizeof(int));
        get_neighs(i, st_len, node_ptr->n_neighs, st, node_ptr->neighs);

        HT_ADD(i, node_ptr);
    }
}

void free_nodes(int n_nodes) {
    for (int i = 0; i < n_nodes; i++) {
        node_t *node_ptr = HT_GET(i);
        if (node_ptr) {
            if (node_ptr->neighs) free(node_ptr->neighs);
            g_free(node_ptr);
        }
    }
}

void free_arr_pointer_int(int n, int *arr[]) {
    for (int i = 0; i < n; i++) {
        if (arr[i]) free(arr[i]);
    }
}

char exceeded_time(time_t ss) {
    return difftime(time(NULL), ss) > TOLERANCE_INTERVAL;
}

int dsatur_compare(const void *a, const void *b) {
    int int_a = *(int *)a;
    int int_b = *(int *)b;

    if (int_a == -1) return INT_MAX;
    else if (int_b == -1) return INT_MIN;

    int satur_a = SATUR_DEGREE(int_a);
    int satur_b = SATUR_DEGREE(int_b);

    return (
        satur_a != satur_b 
        ? -(satur_a - satur_b) 
        : -(HT_GET(int_a)->n_neighs - HT_GET(int_b)->n_neighs)
    );
}

void get_satur(int order_len, int* arr) {
    for (int i = 0; i < order_len; i++) {
        node_t *node_ptr = HT_GET(i);

        for (int j = 0; j < node_ptr->n_neighs; j++) {
            if (arr[node_ptr->neighs[j]] != -1) satur[i]++;
        }
    }
}

void get_order(int n_nodes, int *arr, int node_idx, int* order) {
    int order_len = 0;

    for (int i = 0; i < n_nodes; i++) {
        if (arr[i] != -1 || i == node_idx) continue;

        order[order_len++] = i;
    }

    satur = calloc(order_len, sizeof(int));
    get_satur(order_len, arr);


    qsort(order, order_len, sizeof(int), dsatur_compare);
    free(satur);
}

void recursive(int n_colors, int n_nodes, int depth, int* arr, time_t ss, int node_idx) {
    if (depth == n_nodes) {
        solution_n = n_colors;
        solution = arr;
        return;
    }

    if (depth == 0) arr = malloc_m1(n_nodes, sizeof(int));

    int *order = malloc(n_nodes * sizeof(int));
    get_order(n_nodes, arr, node_idx, order);

    if (depth == 0) {
        arr[order[0]] = 0;
        recursive(n_colors, n_nodes, depth + 1, arr, ss, order[1]);

        if (!solution) free(arr);

        free(order);

        return;
    }

    node_t *node_ptr = HT_GET(node_idx);

    for (int i = 0; i < n_colors; i++) {
        if (is_neigh_color(node_ptr, i, arr)) continue;

        if ((stopped_by_time |= exceeded_time(ss))) break;

        arr[node_idx] = i;
        recursive(n_colors, n_nodes, depth + 1, arr, ss, order[0]);

        if (solution) {
            free(order);
            return;
        }

        arr[order[0]] = -1;
    }

    free(order);
}

double get_inverse_score_array(int n, double *t_arr, int *s_arr) {
    double sum = 0;
    for (int i = 0; i < LOCAL_SEARCH_RANGE; i++) {
        t_arr[i] = (s_arr[i] == 0 ? 0. : (1. / s_arr[i]));
        sum += t_arr[i];
    }

    return sum;
}

void get_partitions(int scores[], double partitions[], int selected) {
    double inv_scores[LOCAL_SEARCH_RANGE];
    double total = get_inverse_score_array(LOCAL_SEARCH_RANGE, inv_scores, scores);

    for (int i = 0; i < LOCAL_SEARCH_RANGE; i++) {
        if (i == selected) {
            partitions[i] = (i > 0 ? partitions[i - 1] : -1.);
            continue;
        }

        partitions[i] = ((i == 0 || partitions[i - 1] == -1.) ? .0 : partitions[i - 1]) + (inv_scores[i] / total);
    }
}

void partition_select(int scores[], int selected[2]) {
    double partitions[LOCAL_SEARCH_RANGE];

    for (int i = 0; i < 2; i++) {
        get_partitions(scores, partitions, i > 0 ? selected[0] : -1);

        int idx = 0;
        double select_partition = ((double)rand() / RAND_MAX) * partitions[LOCAL_SEARCH_RANGE - 1], sum = .0;

        while ((sum += partitions[idx++]) < select_partition);

        selected[i] = idx - 1;
    }
}

int find_first(int n, char* arr, char value) {
    int idx = 0;
    while (idx < n) if (value == arr[idx++]) break;
    return --idx == n ? -1 : idx;
}

void remove_array_dupes(int n, int *arr) {
    char *freq = calloc(n, sizeof(char));
    int *dupe_idxs = malloc(n * sizeof(int)), dupe_idxs_len = 0;

    for (int i = 0; i < n; i++) if (freq[arr[i]]++ > 0) dupe_idxs[dupe_idxs_len++] = i;

    for (int i = 0; i < dupe_idxs_len; i++) {
        int missing = find_first(n, freq, 0);
        arr[dupe_idxs[i]] = missing;

        freq[missing]++;
    }

    free(freq);
    free(dupe_idxs);
}

void crossover(int n, int *selected[], int *offspring[]) {
    for (int i = 0; i < 2; i++) {
        int split_idx = rand() % n;
        char first = rand() % 2;

        memcpy(offspring[i], selected[first], n * sizeof(int));
        memcpy(offspring[i] + split_idx, selected[!first] + split_idx, (n - split_idx) * sizeof(int));

        if (SHOULD_MUTATE) swap(rand() % n, rand() % n, offspring[i]);

        remove_array_dupes(n, offspring[i]);
    }
}

void get_population(int n, int *population[]) {
    for (int j = 0; j < LOCAL_SEARCH_RANGE; j++) {
        int *sample = malloc(n * sizeof(int));
        for (int k = 0; k < n; k++) sample[k] = k;
        shuffle_int_array(n, sample);

        population[j] = sample;
        
    }
}

void choose_random_losers(int n, int winner_idxs[], int selected[]) {
    int arr[LOCAL_SEARCH_RANGE];

    for (int i = 0; i < 2; i++) selected[i] = -1;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < LOCAL_SEARCH_RANGE; j++) {
            if (j == winner_idxs[0] || j == winner_idxs[1] || (selected[0] != -1 ? j == selected[0] : 0)) continue;
            arr[j] = j;
        }

        selected[i] = rand() % (LOCAL_SEARCH_RANGE - (i > 0 ? 3 : 2));
    }
}

void genetic_algorithm(int n) {
    int *population[LOCAL_SEARCH_RANGE];
    get_population(n, population);

    for (int i = 0; i < N_GENERATIONS; i++) {
        int scores[LOCAL_SEARCH_RANGE];
        for (int j = 0; j < LOCAL_SEARCH_RANGE; j++) scores[j] = greedy(n, population[j]);

        int winner_idxs[2];
        partition_select(scores, winner_idxs);

        int *selected[2];
        for (int i = 0; i < 2; i++) selected[i] = population[winner_idxs[i]];

        int *offspring[2];
        for (int i = 0; i < 2; i++) offspring[i] = malloc(n * sizeof(int));
        crossover(n, selected, offspring);

        int loser_idxs[2];
        choose_random_losers(n, winner_idxs, loser_idxs);

        for (int i = 0; i < 2; i++) {
            free(population[loser_idxs[i]]);
            population[loser_idxs[i]] = offspring[i];
        }
    }

    free_arr_pointer_int(LOCAL_SEARCH_RANGE, population);
}

int main() {
    srand(time(NULL));
    int n;
    uint m;
    scanf("%d %u", &n, &m);

    int (*st)[2] = malloc(m * sizeof(*st));
    get_st(m, st);

    ht = g_hash_table_new(g_direct_hash, g_direct_equal);

    insert_nodes(n, m, st);

    free(st);

    (void)genetic_algorithm(n);

    free(last_solution);

    last_solution = solution;
    last_solution_n = solution_n;
    
    time_t ss = time(NULL);

    while (solution && !(stopped_by_time |= exceeded_time(ss))) {
        solution = NULL;
        recursive(--solution_n, n, 0, NULL, ss, -1);

        if (solution) {
            last_solution = solution;
            last_solution_n = solution_n;
        }
    }
    free(solution);

    printf(stopped_by_time ? "%d 0\n" : "%d 1\n", last_solution_n);
    print_int_array(n, last_solution);
    free(last_solution);

    free_nodes(n);
    g_hash_table_destroy(ht);
    return 0;
}
