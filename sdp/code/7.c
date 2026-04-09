#include <stdio.h>
#include <stdbool.h>

int decide_pump_command(int level_pct,
                        bool pump_running,
                        bool manual_on,
                        bool manual_off,
                        bool leak,
                        bool overcurrent,
                        int *out_cmd) {
    if (out_cmd == NULL) {
        return -1;
    }

    /* Clamp level to [0, 100] */
    if (level_pct < 0) level_pct = 0;
    if (level_pct > 100) level_pct = 100;

    /* Priority order */
    if (leak || overcurrent) {
        *out_cmd = 0;                  /* STOP */
    } else if (manual_off) {
        *out_cmd = 0;                  /* STOP */
    } else if (manual_on) {
        *out_cmd = 1;                  /* START */
    } else if (level_pct <= 25) {
        *out_cmd = 1;                  /* START */
    } else if (level_pct >= 80) {
        *out_cmd = 0;                  /* STOP */
    } else {
        *out_cmd = pump_running ? 1 : 0; /* Hold current state */
    }

    return 0;
}

/* ---- Test all 10 cases ---- */
int main(void) {
    int cmd = -1;
    int ret;

    ret = decide_pump_command(15, false, false, false, false, false, &cmd);
    printf("1) ret=%d cmd=%d\n", ret, cmd);  /* 0,1 */

    ret = decide_pump_command(85, true, false, false, false, false, &cmd);
    printf("2) ret=%d cmd=%d\n", ret, cmd);  /* 0,0 */

    ret = decide_pump_command(50, true, false, false, false, false, &cmd);
    printf("3) ret=%d cmd=%d\n", ret, cmd);  /* 0,1 */

    ret = decide_pump_command(50, false, false, false, false, false, &cmd);
    printf("4) ret=%d cmd=%d\n", ret, cmd);  /* 0,0 */

    ret = decide_pump_command(40, false, true, false, false, false, &cmd);
    printf("5) ret=%d cmd=%d\n", ret, cmd);  /* 0,1 */

    ret = decide_pump_command(10, true, false, true, false, false, &cmd);
    printf("6) ret=%d cmd=%d\n", ret, cmd);  /* 0,0 */

    ret = decide_pump_command(70, true, false, false, true, false, &cmd);
    printf("7) ret=%d cmd=%d\n", ret, cmd);  /* 0,0 */

    ret = decide_pump_command(120, false, false, false, false, false, &cmd);
    printf("8) ret=%d cmd=%d\n", ret, cmd);  /* 0,0 */

    ret = decide_pump_command(-5, false, false, false, false, false, &cmd);
    printf("9) ret=%d cmd=%d\n", ret, cmd);  /* 0,1 */

    ret = decide_pump_command(50, false, false, false, false, false, NULL);
    printf("10) ret=%d (NULL out_cmd)\n", ret);  /* -1 */

    return 0;
}
