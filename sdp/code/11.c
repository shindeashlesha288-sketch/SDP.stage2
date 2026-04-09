
#include <stdio.h>

int drain_ring_snapshot(const char *buf,
                        int capacity,
                        int read_index_snapshot,
                        int write_index_snapshot,
                        int max_to_copy,
                        char *out,
                        int *out_count) {
    int i, idx;

    /* Validate (no output write on error) */
    if (buf == NULL || out == NULL || out_count == NULL) return -1;
    if (capacity <= 0 || max_to_copy < 0) return -1;
    if (read_index_snapshot < 0 || read_index_snapshot >= capacity) return -1;
    if (write_index_snapshot < 0 || write_index_snapshot >= capacity) return -1;

    /* One for-loop with two control variables */
    for (i = 0, idx = read_index_snapshot;
         i < max_to_copy && idx != write_index_snapshot;
         i++, idx = (idx + 1) % capacity) {
        out[i] = buf[idx];
    }

    *out_count = i;
    return 0;
}

/* helper to print copied bytes */
static void print_bytes(const char *a, int n) {
    int i;
    for (i = 0; i < n; i++) putchar(a[i]);
}

int main(void) {
    const char buf8[] = {'A','B','C','D','E','F','G','H'};
    const char buf1[] = {'X'};
    char out[32];
    int ret, count;

    ret = drain_ring_snapshot(buf8, 8, 2, 6, 10, out, &count);
    printf("1) ret=%d count=%d out=\"", ret, count); print_bytes(out, count); printf("\"\n");

    ret = drain_ring_snapshot(buf8, 8, 6, 2, 10, out, &count);
    printf("2) ret=%d count=%d out=\"", ret, count); print_bytes(out, count); printf("\"\n");

    ret = drain_ring_snapshot(buf8, 8, 3, 3, 5, out, &count);
    printf("3) ret=%d count=%d\n", ret, count);

    ret = drain_ring_snapshot(buf8, 8, 1, 5, 2, out, &count);
    printf("4) ret=%d count=%d out=\"", ret, count); print_bytes(out, count); printf("\"\n");

    ret = drain_ring_snapshot(buf8, 8, 7, 0, 5, out, &count);
    printf("5) ret=%d count=%d out=\"", ret, count); print_bytes(out, count); printf("\"\n");

    ret = drain_ring_snapshot(buf8, 8, 0, 5, 0, out, &count);
    printf("6) ret=%d count=%d\n", ret, count);

    ret = drain_ring_snapshot(buf1, 1, 0, 0, 3, out, &count);
    printf("7) ret=%d count=%d\n", ret, count);

    ret = drain_ring_snapshot(buf8, 8, 8, 0, 3, out, &count);
    printf("8) ret=%d (invalid read index)\n", ret);

    ret = drain_ring_snapshot(buf8, 0, 0, 0, 3, out, &count);
    printf("9) ret=%d (invalid capacity)\n", ret);

    ret = drain_ring_snapshot(NULL, 8, 0, 0, 3, out, &count);
    printf("10) ret=%d (NULL buf)\n", ret);

    return 0;
}
