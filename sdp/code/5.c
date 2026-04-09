#include <stdio.h>
#include <math.h>

/* Function prototype */
int compute_kmpl(int distance_meters, int fuel_milliliters, double *out_kmpl);

/* Implementation */
int compute_kmpl(int distance_meters, int fuel_milliliters, double *out_kmpl) {
    if (out_kmpl == NULL) return -1;
    if (distance_meters < 0) return -1;
    if (fuel_milliliters <= 0) return -1;

    /* Explicit floating-point math + clear precedence */
    *out_kmpl = (distance_meters / 1000.0) / (fuel_milliliters / 1000.0);
    return 0;
}

/* Simple helper for floating-point compare */
int nearly_equal(double a, double b, double eps) {
    return fabs(a - b) <= eps;
}

int main(void) {
    double out = -1.0;
    int ret;
    int pass = 0;

    /* 1 */
    out = -1.0;
    ret = compute_kmpl(0, 500, &out);
    if (ret == 0 && nearly_equal(out, 0.0, 1e-9)) pass++;
    printf("1) ret=%d, out=%.6f\n", ret, out);

    /* 2 */
    out = -1.0;
    ret = compute_kmpl(50000, 2500, &out);
    if (ret == 0 && nearly_equal(out, 20.0, 1e-9)) pass++;
    printf("2) ret=%d, out=%.6f\n", ret, out);

    /* 3 */
    out = -1.0;
    ret = compute_kmpl(12345, 678, &out);
    if (ret == 0 && nearly_equal(out, 18.2079646018, 1e-6)) pass++;
    printf("3) ret=%d, out=%.6f\n", ret, out);

    /* 4 */
    out = 111.0; /* should remain unchanged on error */
    ret = compute_kmpl(1000, 0, &out);
    if (ret == -1 && nearly_equal(out, 111.0, 1e-9)) pass++;
    printf("4) ret=%d, out=%.6f\n", ret, out);

    /* 5 */
    out = 222.0; /* should remain unchanged on error */
    ret = compute_kmpl(-10, 100, &out);
    if (ret == -1 && nearly_equal(out, 222.0, 1e-9)) pass++;
    printf("5) ret=%d, out=%.6f\n", ret, out);

    /* 6 */
    ret = compute_kmpl(1000, 1000, NULL);
    if (ret == -1) pass++;
    printf("6) ret=%d (NULL pointer test)\n", ret);

    /* 7 */
    out = -1.0;
    ret = compute_kmpl(999, 1000, &out);
    if (ret == 0 && nearly_equal(out, 0.999, 1e-9)) pass++;
    printf("7) ret=%d, out=%.6f\n", ret, out);

    printf("\nPassed %d/7 tests\n", pass);
    return 0;
}
