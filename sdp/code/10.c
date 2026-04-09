#include <stdio.h>

int count_above_threshold(const int *a, int n, int threshold, int *out_count) {
    int i;
    int count = 0;

    /* validation */
    if (a == NULL || out_count == NULL || n < 0) {
        return -1;
    }

    /* classic for loop */
    for (i = 0; i < n; i++) {
        if (a[i] > threshold) {
            count++;
        }
    }

    *out_count = count;
    return 0;
}

/* ---------- tests ---------- */
int main(void) {
    int out = -99;
    int ret;

    int a1[] = {1, 5, 9};
    ret = count_above_threshold(a1, 3, 4, &out);
    printf("1) ret=%d, out=%d\n", ret, out);   /* 2 */

    int a2[] = {10, 10, 10};
    ret = count_above_threshold(a2, 3, 10, &out);
    printf("2) ret=%d, out=%d\n", ret, out);   /* 0 */

    int a3[] = {-1, -5, -3};
    ret = count_above_threshold(a3, 3, -4, &out);
    printf("3) ret=%d, out=%d\n", ret, out);   /* 2 */

    /* empty array: pass any non-NULL pointer when n=0 */
    int dummy = 0;
    ret = count_above_threshold(&dummy, 0, 5, &out);
    printf("4) ret=%d, out=%d\n", ret, out);   /* 0 */

    out = 123;
    ret = count_above_threshold(NULL, 3, 5, &out);
    printf("5) ret=%d, out=%d\n", ret, out);   /* -1, unchanged */

    ret = count_above_threshold(a1, 3, 4, NULL);
    printf("6) ret=%d\n", ret);                /* -1 */

    out = 456;
    ret = count_above_threshold(a1, -1, 4, &out);
    printf("7) ret=%d, out=%d\n", ret, out);   /* -1, unchanged */

    return 0;
}
