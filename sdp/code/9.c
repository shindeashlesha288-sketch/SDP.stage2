
#include <stdio.h>
#include <stdbool.h>

enum Action {
    ACT_NONE = 0,
    ACT_START = 1,
    ACT_STOP = 2,
    ACT_RESET = 3,
    ACT_DIAG = 4,
    ACT_UPDATE = 5,
    ACT_SHUTDOWN = 6
};

int interpret_uart_command(char cmd, bool maintenance_mode, bool locked, enum Action *out_action) {
    if (out_action == NULL) {
        return -1;  /* invalid pointer */
    }

    switch (cmd) {
        case 'S':
            if (locked) return -3;
            *out_action = ACT_START;
            return 0;

        case 'T':
            *out_action = ACT_STOP;
            return 0;

        case 'R':
            *out_action = ACT_RESET;
            return 0;

        case 'D':
            if (!maintenance_mode) return -3;
            if (locked) return -3;
            *out_action = ACT_DIAG;
            return 0;

        case 'U':
            if (!maintenance_mode) return -3;
            if (locked) return -3;
            *out_action = ACT_UPDATE;
            return 0;

        case 'Q':
            if (locked) return -3;
            *out_action = ACT_SHUTDOWN;
            return 0;

        default:
            return -2;  /* unknown command */
    }
}

/* ---------- Test cases ---------- */
int main(void) {
    enum Action a = ACT_NONE;
    int ret;

    ret = interpret_uart_command('S', false, false, &a);
    printf("1) ret=%d action=%d\n", ret, a);   /* 0 START */

    a = ACT_NONE;
    ret = interpret_uart_command('S', false, true, &a);
    printf("2) ret=%d action=%d\n", ret, a);   /* -3 unchanged */

    a = ACT_NONE;
    ret = interpret_uart_command('T', false, true, &a);
    printf("3) ret=%d action=%d\n", ret, a);   /* 0 STOP */

    a = ACT_NONE;
    ret = interpret_uart_command('R', false, true, &a);
    printf("4) ret=%d action=%d\n", ret, a);   /* 0 RESET */

    a = ACT_NONE;
    ret = interpret_uart_command('D', false, false, &a);
    printf("5) ret=%d action=%d\n", ret, a);   /* -3 unchanged */

    a = ACT_NONE;
    ret = interpret_uart_command('D', true, false, &a);
    printf("6) ret=%d action=%d\n", ret, a);   /* 0 DIAG */

    a = ACT_NONE;
    ret = interpret_uart_command('U', true, true, &a);
    printf("7) ret=%d action=%d\n", ret, a);   /* -3 unchanged */

    a = ACT_NONE;
    ret = interpret_uart_command('Q', false, false, &a);
    printf("8) ret=%d action=%d\n", ret, a);   /* 0 SHUTDOWN */

    a = ACT_NONE;
    ret = interpret_uart_command('Q', false, true, &a);
    printf("9) ret=%d action=%d\n", ret, a);   /* -3 unchanged */

    a = ACT_NONE;
    ret = interpret_uart_command('X', false, false, &a);
    printf("10) ret=%d action=%d\n", ret, a);  /* -2 unchanged */

    ret = interpret_uart_command('S', false, false, NULL);
    printf("11) ret=%d\n", ret);               /* -1 */

    return 0;
}
