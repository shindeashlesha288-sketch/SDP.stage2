#include <stdio.h>
#include <stdbool.h>

static int iabs_int(int x) { return (x < 0) ? -x : x; }

int find_longest_stable_segment(
    const int *a,
    int n,
    int step_max,
    int span_max,
    double avg_min,
    double avg_max,
    int min_len,
    int max_len,          /* 0 => no upper bound */
    int *out_start,
    int *out_end,
    int *out_len,
    double *out_avg
) {
    int L, R;
    int best_L = -1, best_R = -1, best_len = 0;
    double best_avg = 0.0;

    /* Validate */
    if (!a || !out_start || !out_end || !out_len || !out_avg) return -1;
    if (n < 0 || step_max < 0 || span_max < 0) return -1;
    if (min_len < 1) return -1;
    if (avg_min > avg_max) return -1;
    if (max_len > 0 && max_len < min_len) return -1;

    if (n == 0) return -2;

    for (L = 0; L < n; L++) {
        long sum = 0;
        int vmin = a[L], vmax = a[L];

        for (R = L; R < n; R++) {
            int len;
            double mean;

            /* Step constraint: if broken, longer R for same L won't work */
            if (R > L && iabs_int(a[R] - a[R - 1]) > step_max) {
                break;
            }

            sum += (long)a[R];
            if (a[R] < vmin) vmin = a[R];
            if (a[R] > vmax) vmax = a[R];

            len = R - L + 1;

            if (max_len > 0 && len > max_len) {
                break;
            }

            if (len < min_len) {
                continue;
            }

            if ((vmax - vmin) > span_max) {
                continue;
            }

            mean = (double)sum / (double)len;
            if (mean < avg_min || mean > avg_max) {
                continue;
            }

            /* Tie-breakers: longer > higher avg > earlier L */
            if (len > best_len ||
                (len == best_len && mean > best_avg) ||
                (len == best_len && mean == best_avg && L < best_L)) {
                best_len = len;
                best_L = L;
                best_R = R;
                best_avg = mean;
            }
        }
    }

    if (best_len == 0) return -2;

    *out_start = best_L;
    *out_end = best_R;
    *out_len = best_len;
    *out_avg = best_avg;
    return 0;
}

/* ---------------- Tests ---------------- */
static void run_test(
    int tc,
    const int *a, int n, int step_max, int span_max,
    double avg_min, double avg_max, int min_len, int max_len
) {
    int s = -99, e = -99, len = -99, ret;
    double avg = -999.0;

    ret = find_longest_stable_segment(
        a, n, step_max, span_max, avg_min, avg_max, min_len, max_len,
        &s, &e, &len, &avg
    );

    if (ret == 0) {
        printf("%d) ret=%d start=%d end=%d len=%d avg=%.6f\n",
               tc, ret, s, e, len, avg);
    } else {
        printf("%d) ret=%d\n", tc, ret);
    }
}

int main(void) {
    int a1[] = {10,12,13,13,15,20};
    int a2[] = {5,5,5,5};
    int a3[] = {1,20,2,21,3,22};
    int a4[] = {3,4,7,8,9};
    int a5[] = {100};
    int a6[] = {-2,-1,-1,0,1};

    run_test(1, a1, 6, 3, 6, 11.0, 15.0, 3, 0);   /* expect 0..4 len 5 avg 12.6 */
    run_test(2, a2, 4, 0, 0, 5.0, 5.0, 2, 0);      /* expect 0..3 len 4 avg 5.0 */
    run_test(3, a3, 6, 3, 5, 0.0, 100.0, 2, 0);    /* expect 4..5 len 2 avg 12.5 */
    run_test(4, a4, 5, 4, 3, 5.0, 7.0, 2, 3);      /* expect 2..3 len 2 avg 7.5 */
    run_test(5, a5, 1, 0, 0, 200.0, 300.0, 1, 0);  /* expect -2 */
    run_test(6, a6, 5, 2, 3, -1.5, 0.5, 3, 0);     /* expect 0..4 len 5 avg -0.6 */
    run_test(7, a1, 0, 1, 1, 0.0, 1.0, 1, 0);      /* expect -2 (n=0) */
    run_test(8, NULL, 6, 1, 1, 0.0, 1.0, 1, 0);    /* expect -1 */

    /* invalid params sample */
    run_test(9, a1, 6, 1, 1, 2.0, 1.0, 1, 0);      /* avg_min > avg_max => -1 */

    return 0;
}
