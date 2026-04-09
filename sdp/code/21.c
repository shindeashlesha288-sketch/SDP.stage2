#include <stdio.h>

#define INSERTION_SORT_THRESHOLD 16

static void insertion_sort_range(int *a, int left, int right) {
    int i, j, key;
    for (i = left + 1; i <= right; i++) {
        key = a[i];
        j = i - 1;
        /* stable: move only strictly greater values */
        while (j >= left && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

static void merge_range(int *a, int left, int mid, int right, int *scratch) {
    int i = left;
    int j = mid + 1;
    int k = 0;
    int len = right - left + 1;
    int t;

    while (i <= mid && j <= right) {
        /* stable: take left on equal */
        if (a[i] <= a[j]) scratch[k++] = a[i++];
        else              scratch[k++] = a[j++];
    }
    while (i <= mid)   scratch[k++] = a[i++];
    while (j <= right) scratch[k++] = a[j++];

    for (t = 0; t < len; t++) {
        a[left + t] = scratch[t];
    }
}

static void mergesort_rec(int *a, int left, int right, int *scratch) {
    int len = right - left + 1;
    int mid;

    if (left >= right) return;

    if (len <= INSERTION_SORT_THRESHOLD) {
        insertion_sort_range(a, left, right);
        return;
    }

    mid = left + (right - left) / 2; /* overflow-safe */
    mergesort_rec(a, left, mid, scratch);
    mergesort_rec(a, mid + 1, right, scratch);
    merge_range(a, left, mid, right, scratch);
}

int stable_sort_range_with_scratch(int *a,
                                   int n,
                                   int left,
                                   int right,
                                   int *scratch,
                                   int scratch_capacity,
                                   int *out_sorted_count) {
    int m;

    /* Validate first: no output writes on error */
    if (a == NULL || scratch == NULL || out_sorted_count == NULL) return -1;
    if (n < 0) return -1;
    if (scratch_capacity < 0) return -1;

    if (n == 0) {
        *out_sorted_count = 0;
        return 0;
    }

    if (left < 0 || right < 0 || left > right || right >= n) return -1;

    m = right - left + 1;
    if (scratch_capacity < m) return -1;

    mergesort_rec(a, left, right, scratch);

    *out_sorted_count = m;
    return 0;
}

/* -------- tests -------- */
static void print_arr(const int *a, int n) {
    int i;
    printf("[");
    for (i = 0; i < n; i++) {
        printf("%d", a[i]);
        if (i + 1 < n) printf(",");
    }
    printf("]");
}

int main(void) {
    int out, ret;
    int scratch[16];

    int a1[] = {5,3,3,2,9};
    ret = stable_sort_range_with_scratch(a1, 5, 0, 4, scratch, 5, &out);
    printf("1) ret=%d out=%d a=", ret, out); print_arr(a1, 5); printf("\n");

    int a2[] = {4,1,4,2,4};
    ret = stable_sort_range_with_scratch(a2, 5, 1, 3, scratch, 3, &out);
    printf("2) ret=%d out=%d a=", ret, out); print_arr(a2, 5); printf("\n");

    int a3[] = {3,3,3};
    ret = stable_sort_range_with_scratch(a3, 3, 0, 2, scratch, 3, &out);
    printf("3) ret=%d out=%d a=", ret, out); print_arr(a3, 3); printf("\n");

    int a4[] = {1,2,3,4};
    ret = stable_sort_range_with_scratch(a4, 4, 0, 3, scratch, 4, &out);
    printf("4) ret=%d out=%d a=", ret, out); print_arr(a4, 4); printf("\n");

    int a5[] = {4,3,2,1};
    ret = stable_sort_range_with_scratch(a5, 4, 0, 3, scratch, 4, &out);
    printf("5) ret=%d out=%d a=", ret, out); print_arr(a5, 4); printf("\n");

    int a6[] = {9,8,7,6};
    out = 999;
    ret = stable_sort_range_with_scratch(a6, 4, 0, 3, scratch, 2, &out);
    printf("6) ret=%d out=%d\n", ret, out);

    ret = stable_sort_range_with_scratch(NULL, 4, 0, 3, scratch, 4, &out);
    printf("7) ret=%d\n", ret);

    ret = stable_sort_range_with_scratch(a6, 0, 0, 0, scratch, 4, &out);
    printf("8) ret=%d out=%d\n", ret, out);

    ret = stable_sort_range_with_scratch(a6, 4, 2, 1, scratch, 4, &out);
    printf("9) ret=%d\n", ret);

    return 0;
}
