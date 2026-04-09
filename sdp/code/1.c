#include <stdio.h>

int adc_to_percent(int adc, int adc_max, int *out_percent) {
    long temp;
    int percent;

    if (out_percent == NULL) return -1;
    if (adc_max <= 0 || adc < 0 || adc > adc_max) return -1;

    temp = (long)adc * 100;                           // safe multiply
    percent = (int)((temp + adc_max / 2) / adc_max); // round to nearest

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    *out_percent = percent;
    return 0;
}

int main() {
    int adc_vals[7]     = {0, 4095, 2048, 1023, 1100, 500, 205};
    int adc_max_vals[7] = {4095, 4095, 4095, 1023, 1023, 0, 4095};
    int expected_ret[7] = {0, 0, 0, 0, -1, -1, 0};
    int expected_pct[7] = {0, 100, 50, 100, -1, -1, 5}; // -1 = not applicable

    int i, out, ret;

    for (i = 0; i < 7; i++) {
        out = -1;
        ret = adc_to_percent(adc_vals[i], adc_max_vals[i], &out);

        printf("Case %d: adc=%d, adc_max=%d -> ret=%d",
               i + 1, adc_vals[i], adc_max_vals[i], ret);

        if (ret == 0) {
            printf(", percent=%d", out);
        }

        // check against expected
        if (ret == expected_ret[i] &&
            (ret != 0 || out == expected_pct[i])) {
            printf(" [PASS]");
        } else {
            printf(" [FAIL]");
        }

        printf("\n");
    }

    return 0;
}
