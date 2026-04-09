
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    RC_NOFAULT  = 0,
    RC_SEEN_ONLY = 1,
    RC_CLEARED  = 2,
    RC_BAD_ARG  = -1
} RetCode;

/* Provided by system */
extern long g_ms_tick;

int handle_w1c_fault(volatile int *reg,
                     int fault_bit,
                     bool allow_clear,
                     long cooldown_ms,
                     int *out_seen_count) {
    static int seen_count = 0;
    static long last_clear_ms = 0;
    static bool has_cleared = false;

    int val;
    int mask;
    bool cooldown_ok;

    if (reg == NULL || out_seen_count == NULL) return RC_BAD_ARG;
    if (fault_bit < 0 || fault_bit > 30) return RC_BAD_ARG;
    if (cooldown_ms < 0) return RC_BAD_ARG;

    mask = (int)(1u << fault_bit);

    /* Read MMIO once */
    val = *reg;

    if ((val & mask) == 0) {
        *out_seen_count = seen_count;
        return RC_NOFAULT;
    }

    seen_count++;

    if (!has_cleared) {
        cooldown_ok = true; /* first clear allowed */
    } else {
        cooldown_ok = ((g_ms_tick - last_clear_ms) >= cooldown_ms);
    }

    if (allow_clear && cooldown_ok) {
        *reg = mask;               /* W1C write: only fault bit = 1 */
        last_clear_ms = g_ms_tick;
        has_cleared = true;
        *out_seen_count = seen_count;
        return RC_CLEARED;
    }

    *out_seen_count = seen_count;
    return RC_SEEN_ONLY;
}

/* --------- demo tests --------- */
long g_ms_tick = 0;

int main(void) {
    volatile int reg = 0;
    int seen = -1;
    int rc;

    reg = 0x00; g_ms_tick = 0;
    rc = handle_w1c_fault(&reg, 3, true, 0, &seen);
    printf("1) rc=%d seen=%d reg=0x%X\n", rc, seen, reg);

    reg = 0x08; g_ms_tick = 10;
    rc = handle_w1c_fault(&reg, 3, false, 0, &seen);
    printf("2) rc=%d seen=%d reg=0x%X\n", rc, seen, reg);

    reg = 0x08; g_ms_tick = 100;
    rc = handle_w1c_fault(&reg, 3, true, 0, &seen);
    printf("3) rc=%d seen=%d reg_write=0x%X\n", rc, seen, reg);

    reg = 0x08; g_ms_tick = 120;
    rc = handle_w1c_fault(&reg, 3, true, 50, &seen);
    printf("4) rc=%d seen=%d reg=0x%X\n", rc, seen, reg);

    reg = 0x08; g_ms_tick = 180;
    rc = handle_w1c_fault(&reg, 3, true, 50, &seen);
    printf("5) rc=%d seen=%d reg_write=0x%X\n", rc, seen, reg);

    rc = handle_w1c_fault(NULL, 3, true, 0, &seen);
    printf("6a) rc=%d\n", rc);

    rc = handle_w1c_fault(&reg, 31, true, 0, &seen);
    printf("6b) rc=%d\n", rc);

    rc = handle_w1c_fault(&reg, 3, true, 0, NULL);
    printf("6c) rc=%d\n", rc);

    return 0;
}
