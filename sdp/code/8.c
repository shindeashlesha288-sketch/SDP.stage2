#include <stdio.h>
#include <stdbool.h>

enum AdmissionReason {
    REASON_ACCEPTED = 0,
    REASON_MAINTENANCE = 1,
    REASON_COOLDOWN = 2,
    REASON_MIN_GAP = 3,
    REASON_WINDOW_LIMIT = 4
};

int decide_admission(
    long now_ms,
    long *window_start_ms,
    int window_ms,
    int max_in_window,
    long min_gap_ms,
    long cooldown_ms,
    long *last_accept_ms,
    long *cooldown_until_ms,
    int *count_in_window,
    bool maintenance_mode,
    bool emergency_override,
    int *out_decision,                 /* 0=REJECT, 1=ACCEPT */
    enum AdmissionReason *out_reason
) {
    /* Validate: do not modify outputs/state on error */
    if (!window_start_ms || !last_accept_ms || !cooldown_until_ms ||
        !count_in_window || !out_decision || !out_reason) return -1;

    if (now_ms < 0 || *window_start_ms < 0 || *cooldown_until_ms < 0) return -1;
    if (window_ms <= 0 || max_in_window < 1 || min_gap_ms < 0 || cooldown_ms < 0) return -1;
    if (*count_in_window < 0) return -1;
    if (*last_accept_ms < -1) return -1;

    /* monotonic sanity checks */
    if (now_ms < *window_start_ms) return -1;
    if (*last_accept_ms >= 0 && now_ms < *last_accept_ms) return -1;

    /* 1) maintenance */
    if (maintenance_mode && !emergency_override) {
        *out_decision = 0;
        *out_reason = REASON_MAINTENANCE;
        return 0;
    }
    /* 2) cooldown */
    else if (now_ms < *cooldown_until_ms) {
        *out_decision = 0;
        *out_reason = REASON_COOLDOWN;
        return 0;
    }
    /* 3) min-gap */
    else if (*last_accept_ms >= 0 && (now_ms - *last_accept_ms) < min_gap_ms) {
        *out_decision = 0;
        *out_reason = REASON_MIN_GAP;
        return 0;
    }

    /* 4) window roll */
    if (now_ms >= (*window_start_ms + (long)window_ms)) {
        *window_start_ms = now_ms;
        *count_in_window = 0;
    }

    /* 5) window limit */
    if (*count_in_window >= max_in_window) {
        *cooldown_until_ms = now_ms + cooldown_ms;  /* one-shot cooldown */
        *out_decision = 0;
        *out_reason = REASON_WINDOW_LIMIT;
        return 0;
    }

    /* Accept */
    *last_accept_ms = now_ms;
    (*count_in_window)++;
    *out_decision = 1;
    *out_reason = REASON_ACCEPTED;
    return 0;
}

/* ---------------- simple tests ---------------- */
int main(void) {
    long window_start = 1000;
    long last_accept = -1;
    long cooldown_until = 0;
    int count = 0;

    int decision;
    enum AdmissionReason reason;
    int ret;

    ret = decide_admission(1000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("1) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(1200, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("2) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(1700, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("3) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(2300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("4) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(2400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("5) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(5300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("6) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(5400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("7) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld\n", ret, decision, reason, last_accept, count, cooldown_until);

    ret = decide_admission(11000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("8) ret=%d dec=%d reason=%d last=%ld count=%d cd=%ld win_start=%ld\n", ret, decision, reason, last_accept, count, cooldown_until, window_start);

    ret = decide_admission(12000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, false, &decision, &reason);
    printf("9) ret=%d dec=%d reason=%d\n", ret, decision, reason);

    ret = decide_admission(12550, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, true, &decision, &reason);
    printf("10) ret=%d dec=%d reason=%d\n", ret, decision, reason);

    return 0;
}
