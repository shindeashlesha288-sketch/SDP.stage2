#include <stdio.h>
#include <stdbool.h>

/* Enums */
enum LockState {
    ST_LOCKED = 0,
    ST_UNLOCKED,
    ST_MOVING_LOCK,
    ST_MOVING_UNLOCK,
    ST_ALARM,
    ST_LOWPOWER,
    ST_LOCKSTATE_COUNT
};

enum LockEvent {
    EV_LOCK_CMD = 0,
    EV_UNLOCK_CMD,
    EV_MOTOR_DONE_OK,
    EV_MOTOR_STALL,
    EV_FORCED_OPEN,
    EV_TIMEOUT,
    EV_BATTERY_LOW,
    EV_BATTERY_OK,
    EV_LOCKEVENT_COUNT
};

enum Action {
    ACT_NONE = 0,
    ACT_START_LOCK,
    ACT_START_UNLOCK,
    ACT_SOUND_ALARM,
    ACT_ENTER_SLEEP,
    ACT_ACTION_COUNT
};

/* Helpers */
static bool is_valid_state(enum LockState s) {
    return (s >= 0 && s < ST_LOCKSTATE_COUNT);
}
static bool is_valid_event(enum LockEvent e) {
    return (e >= 0 && e < EV_LOCKEVENT_COUNT);
}
static bool is_moving_state(enum LockState s) {
    return (s == ST_MOVING_LOCK || s == ST_MOVING_UNLOCK);
}
static bool is_non_moving_state(enum LockState s) {
    return (s == ST_LOCKED || s == ST_UNLOCKED || s == ST_ALARM || s == ST_LOWPOWER);
}

/* Main function */
int lock_step(enum LockState curr,
              enum LockEvent ev,
              bool battery_low,
              int attempts,
              enum LockState *out_next,
              enum Action *out_action,
              int *out_attempts_next) {
    enum LockState next;
    enum Action action;
    int attempts_next;

    /* Validation: no output write on error */
    if (out_next == NULL || out_action == NULL || out_attempts_next == NULL) return -1;
    if (!is_valid_state(curr) || !is_valid_event(ev)) return -1;
    if (attempts < 0) return -1;

    next = curr;
    action = ACT_NONE;
    attempts_next = attempts;

    /* Guard 1: battery low event (except alarm) */
    if (battery_low && ev == EV_BATTERY_LOW && curr != ST_ALARM) {
        next = ST_LOWPOWER;
        action = ACT_ENTER_SLEEP;
    }
    /* Guard 2: recover from low power */
    else if (curr == ST_LOWPOWER && ev == EV_BATTERY_OK) {
        next = ST_UNLOCKED;
        action = ACT_NONE;
    }
    /* Guard 3: forced open => alarm */
    else if (ev == EV_FORCED_OPEN) {
        next = ST_ALARM;
        action = ACT_SOUND_ALARM;
    }
    /* Moving state handling */
    else if (curr == ST_MOVING_LOCK || curr == ST_MOVING_UNLOCK) {
        switch (ev) {
            case EV_MOTOR_DONE_OK:
                next = (curr == ST_MOVING_LOCK) ? ST_LOCKED : ST_UNLOCKED;
                action = ACT_NONE;
                break;
            case EV_MOTOR_STALL:
            case EV_TIMEOUT:
                next = ST_ALARM;
                action = ACT_SOUND_ALARM;
                break;
            default:
                break;
        }
    }
    /* Command handling */
    else if (ev == EV_LOCK_CMD || ev == EV_UNLOCK_CMD) {
        if (attempts >= 3) {
            /* ignored */
            next = curr;
            action = ACT_NONE;
            attempts_next = attempts;
        } else {
            if (curr == ST_UNLOCKED && ev == EV_LOCK_CMD) {
                next = ST_MOVING_LOCK;
                action = ACT_START_LOCK;
                attempts_next = attempts + 1;
            } else if (curr == ST_LOCKED && ev == EV_UNLOCK_CMD) {
                next = ST_MOVING_UNLOCK;
                action = ACT_START_UNLOCK;
                attempts_next = attempts + 1;
            }
        }
    }

    /* Reset attempts on transition to non-moving state */
    if (next != curr && is_non_moving_state(next)) {
        attempts_next = 0;
    }
    /* Also reset on successful motor completion */
    if (ev == EV_MOTOR_DONE_OK && is_moving_state(curr)) {
        attempts_next = 0;
    }

    *out_next = next;
    *out_action = action;
    *out_attempts_next = attempts_next;
    return 0;
}

/* ---------- Tests ---------- */

typedef struct {
    enum LockState curr;
    enum LockEvent ev;
    bool battery_low;
    int attempts;
    int exp_ret;
    enum LockState exp_next;
    enum Action exp_action;
    int exp_attempts;
    bool check_outputs;
    const char *name;
} TestCase;

int main(void) {
    TestCase tests[] = {
        {ST_UNLOCKED, EV_LOCK_CMD, false, 0, 0, ST_MOVING_LOCK,   ACT_START_LOCK, 1, true,  "1 Normal lock command"},
        {ST_MOVING_LOCK, EV_MOTOR_DONE_OK, false, 1, 0, ST_LOCKED, ACT_NONE,       0, true,  "2 Motor completes lock"},
        {ST_LOCKED, EV_UNLOCK_CMD, false, 3, 0, ST_LOCKED,         ACT_NONE,       3, true,  "3 Attempt limit ignore"},
        {ST_MOVING_UNLOCK, EV_TIMEOUT, false, 1, 0, ST_ALARM,      ACT_SOUND_ALARM,0, true,  "4 Timeout while moving"},
        {ST_LOCKED, EV_FORCED_OPEN, false, 0, 0, ST_ALARM,         ACT_SOUND_ALARM,0, true,  "5 Forced open"},
        {ST_UNLOCKED, EV_BATTERY_LOW, true, 0, 0, ST_LOWPOWER,     ACT_ENTER_SLEEP,0, true,  "6 Enter low-power"},
        {ST_LOWPOWER, EV_BATTERY_OK, false, 0, 0, ST_UNLOCKED,     ACT_NONE,       0, true,  "7 Recover low-power"},
        {(enum LockState)99, EV_LOCK_CMD, false, 0, -1, ST_LOCKED, ACT_NONE,       0, false, "8 Invalid state enum"},
        {ST_LOCKED, (enum LockEvent)99, false, 0, -1, ST_LOCKED,   ACT_NONE,       0, false, "9 Invalid event enum"},
        {ST_LOCKED, EV_LOCK_CMD, false, -1, -1, ST_LOCKED,         ACT_NONE,       0, false, "10 Invalid attempts"},
    };

    int i, n = (int)(sizeof(tests) / sizeof(tests[0]));
    int pass = 0;

    for (i = 0; i < n; i++) {
        enum LockState next = ST_ALARM; /* sentinel */
        enum Action action = ACT_ENTER_SLEEP; /* sentinel */
        int attempts_next = 777; /* sentinel */

        int ret = lock_step(tests[i].curr, tests[i].ev, tests[i].battery_low, tests[i].attempts,
                            &next, &action, &attempts_next);

        bool ok = (ret == tests[i].exp_ret);

        if (ok && tests[i].check_outputs) {
            ok = (next == tests[i].exp_next &&
                  action == tests[i].exp_action &&
                  attempts_next == tests[i].exp_attempts);
        }

        printf("%s: %s\n", tests[i].name, ok ? "PASS" : "FAIL");
        if (!ok) {
            printf("   got ret=%d next=%d action=%d attempts=%d\n",
                   ret, (int)next, (int)action, attempts_next);
        } else {
            pass++;
        }
    }

    /* null pointer test */
    {
        enum LockState keep_next = ST_LOCKED;
        enum Action keep_action = ACT_NONE;
        int keep_attempts = 5;
        int ret = lock_step(ST_LOCKED, EV_UNLOCK_CMD, false, 0,
                            NULL, &keep_action, &keep_attempts);
        bool ok = (ret == -1 && keep_next == ST_LOCKED &&
                   keep_action == ACT_NONE && keep_attempts == 5);
        printf("11 Invalid pointer: %s\n", ok ? "PASS" : "FAIL");
        if (ok) pass++;
    }

    printf("\nPassed %d/%d tests\n", pass, n + 1);
    return 0;
}
