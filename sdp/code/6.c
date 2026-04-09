#include <stdio.h>

int consume_below_threshold(const int *a, int n, int *index,
                            int threshold, int limit,
                            int *out_sum, int *out_count) {
    int sum = 0;
    int cnt = 0;
    int i;

    /* Validate first: no output modification on error */
    if (a == NULL || index == NULL || out_sum == NULL || out_count == NULL) return -1;
    if (n < 0 || limit < 0) return -1;
    if (*index < 0 || *index > n) return -1;

    i = *index;

    /* Short-circuit order prevents out-of-bounds read */
    while (cnt < limit && i < n && a[i] < threshold) {
        sum += a[i];
        i++;      /* advance index only after consuming */
        cnt++;
    }

    *index = i;
    *out_sum = sum;
    *out_count = cnt;
    return 0;
}

/* ------------------ tests ------------------ */
static void print_result(int tc, int ret, int sum, int cnt, int idx) {
    printf("Case %d: ret=%d, sum=%d, count=%d, index=%d\n", tc, ret, sum, cnt, idx);
}

int main(void) {
    int ret, sum, cnt, idx;

    /* 1 */
    {
        int a[] = {1,2,3,9,1};
        idx = 0; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 5, &idx, 5, 10, &sum, &cnt);
        print_result(1, ret, sum, cnt, idx); /* 0,6,3,3 */
    }

    /* 2 */
    {
        int a[] = {-1,-2,-3};
        idx = 0; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 3, &idx, 0, 2, &sum, &cnt);
        print_result(2, ret, sum, cnt, idx); /* 0,-3,2,2 */
    }

    /* 3 */
    {
        int a[] = {1,2,3};
        idx = 3; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 3, &idx, 10, 5, &sum, &cnt);
        print_result(3, ret, sum, cnt, idx); /* 0,0,0,3 */
    }

    /* 4 */
    {
        int a[] = {4,4,4};
        idx = 0; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 3, &idx, 4, 5, &sum, &cnt);
        print_result(4, ret, sum, cnt, idx); /* 0,0,0,0 */
    }

    /* 5 */
    {
        int a[] = {1,100,2};
        idx = 0; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 3, &idx, 50, 5, &sum, &cnt);
        print_result(5, ret, sum, cnt, idx); /* 0,1,1,1 */
    }

    /* 6 */
    {
        int a[] = {1,2,3};
        idx = 0; sum = -1; cnt = -1;
        ret = consume_below_threshold(a, 3, &idx, 5, 0, &sum, &cnt);
        print_result(6, ret, sum, cnt, idx); /* 0,0,0,0 */
    }

    /* 7: a = NULL (must not modify outputs) */
    {
        idx = 0; sum = 111; cnt = 222;
        ret = consume_below_threshold(NULL, 3, &idx, 5, 1, &sum, &cnt);
        print_result(7, ret, sum, cnt, idx); /* -1,111,222,0 */
    }

    /* 8: n < 0 (must not modify outputs) */
    {
        int a[] = {1};
        idx = 0; sum = 333; cnt = 444;
        ret = consume_below_threshold(a, -1, &idx, 5, 1, &sum, &cnt);
        print_result(8, ret, sum, cnt, idx); /* -1,333,444,0 */
    }

    return 0;
}
