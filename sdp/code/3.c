#include <stdio.h>
#include <stdbool.h>

enum Mode {
    MODE_SLEEP,
    MODE_IDLE,
    MODE_RUN,
    MODE_BOOST
};

int mode_to_cpu_mhz(enum Mode mode, bool power_save) {
    int base_mhz;

    switch (mode) {
        case MODE_SLEEP: base_mhz = 0;  break;
        case MODE_IDLE:  base_mhz = 24; break;
        case MODE_RUN:   base_mhz = 48; break;
        case MODE_BOOST: base_mhz = 96; break;
        default: return -1; /* invalid mode */
    }

    if (mode == MODE_SLEEP) return 0;   /* always 0 */
    if (power_save) return base_mhz / 2;

    return base_mhz;
}

int main(void) {
    printf("1) %d\n", mode_to_cpu_mhz(MODE_SLEEP, false));     /* 0 */
    printf("2) %d\n", mode_to_cpu_mhz(MODE_IDLE,  false));     /* 24 */
    printf("3) %d\n", mode_to_cpu_mhz(MODE_RUN,   true));      /* 24 */
    printf("4) %d\n", mode_to_cpu_mhz(MODE_BOOST, true));      /* 48 */
    printf("5) %d\n", mode_to_cpu_mhz((enum Mode)99, false));  /* -1 */
    printf("6) %d\n", mode_to_cpu_mhz(MODE_SLEEP, true));      /* 0 */

    return 0;
}
