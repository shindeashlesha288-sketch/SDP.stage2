#include <stdio.h>

int find_min_in_array(const int *a, int n, int *out_min) {
    int i;
    int min_value;

    /* validation */
    if (a == NULL || out_min == NULL || n <= 0) {
        return -1;
    }

    min_value = a[0];

    for (i = 1; i < n; i++) {
        if (a[i] < min_value) {
            min_value = a[i];
        }
    }

    *out_min = min_value;
    return 0;
}

/* tests */
int main(void) {
    int ret, out = 999;

    int a1[] = {5, 2, 8, 1, 9};
    ret = find_min_in_array(a1, 5, &out);
    printf("1) ret=%d, min=%d\n", ret, out); /* 0,1 */

    int a2[] = {10};
    ret = find_min_in_array(a2, 1, &out);
    printf("2) ret=%d, min=%d\n", ret, out); /* 0,10 */

    int a3[] = {-5, -2, -9, -1};
    ret = find_min_in_array(a3, 4, &out);
    printf("3) ret=%d, min=%d\n", ret, out); /* 0,-9 */

    int a4[] = {100, 100, 100};
    ret = find_min_in_array(a4, 3, &out);
    printf("4) ret=%d, min=%d\n", ret, out); /* 0,100 */

    out = 123;
    ret = find_min_in_array(a1, 0, &out);
    printf("5) ret=%d, min=%d\n", ret, out); /* -1, unchanged */

    out = 456;
    ret = find_min_in_array(NULL, 5, &out);
    printf("6) ret=%d, min=%d\n", ret, out); /* -1, unchanged */

    ret = find_min_in_array(a1, 5, NULL);
    printf("7) ret=%d\n", ret); /* -1 */

    return 0;
}
