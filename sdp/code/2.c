#include <stdio.h>
#include <stdbool.h>

int calib_map_linear(int x, int x1, int y1, int x2, int y2,
                     bool clamp, int y_min, int y_max, int *out_y) {
    long num, den, term, y;

    /* Validate */
    if (out_y == NULL) return -1;
    if (x1 == x2) return -1;
    if (clamp && y_min > y_max) return -1;

    /* Linear map: y = y1 + (x-x1)*(y2-y1)/(x2-x1) */
    num = (long)(x - x1) * (long)(y2 - y1);
    den = (long)(x2 - x1);

    /* keep denominator positive */
    if (den < 0) {
        den = -den;
        num = -num;
    }

    /* round to nearest */
    if (num >= 0)
        term = (num + den / 2) / den;
    else
        term = (num - den / 2) / den;

    y = (long)y1 + term;

    /* optional clamp */
    if (clamp) {
        if (y < y_min) y = y_min;
        if (y > y_max) y = y_max;
    }

    *out_y = (int)y;
    return 0;
}

int main(void) {
    int out, ret;

    /* 1 */
    ret = calib_map_linear(75, 50, 0, 100, 100, false, 0, 0, &out);
    printf("1) ret=%d out=%d\n", ret, out);

    /* 2 */
    ret = calib_map_linear(100, 50, 0, 100, 100, false, 0, 0, &out);
    printf("2) ret=%d out=%d\n", ret, out);

    /* 3 */
    ret = calib_map_linear(120, 50, 0, 100, 100, true, 0, 100, &out);
    printf("3) ret=%d out=%d\n", ret, out);

    /* 4 */
    ret = calib_map_linear(40, 50, 0, 100, 100, true, 0, 100, &out);
    printf("4) ret=%d out=%d\n", ret, out);

    /* 5 */
    ret = calib_map_linear(60, 100, 200, 200, 400, false, 0, 0, &out);
    printf("5) ret=%d out=%d\n", ret, out);

    /* 6 */
    ret = calib_map_linear(10, 10, 500, 10, 900, false, 0, 0, &out);
    printf("6) ret=%d\n", ret);

    /* 7 */
    ret = calib_map_linear(75, 50, 0, 100, 100, false, 0, 0, NULL);
    printf("7) ret=%d\n", ret);

    return 0;
}
