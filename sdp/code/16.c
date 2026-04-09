
#include <stdio.h>
#include <stdbool.h>

int debounce_and_fire(const int *samples,
                      int n,
                      int k,
                      void (*on_press)(int event_code),
                      bool *out_invoked) {
    int i;
    bool all_one = true;

    /* Validate: no output changes on error */
    if (samples == NULL || on_press == NULL || out_invoked == NULL) return -1;
    if (n < 0 || k < 1) return -1;

    if (n < k) {
        *out_invoked = false;
        return 0;
    }

    for (i = n - k; i < n; i++) {
        if (samples[i] != 1) {
            all_one = false;
            break;
        }
    }

    if (all_one) {
        on_press(1);              /* fire exactly once */
        *out_invoked = true;
    } else {
        *out_invoked = false;
    }

    return 0;
}

/* -------- test callback -------- */
static int g_callback_count = 0;
static int g_last_event = 0;

void my_press_handler(int event_code) {
    g_callback_count++;
    g_last_event = event_code;
    printf("  callback fired: event_code=%d\n", event_code);
}

int main(void) {
    int ret;
    bool invoked;

    /* 1 */
    int s1[] = {0,1,1,1};
    g_callback_count = 0; g_last_event = 0;
    ret = debounce_and_fire(s1, 4, 3, my_press_handler, &invoked);
    printf("1) ret=%d invoked=%d calls=%d event=%d\n", ret, invoked, g_callback_count, g_last_event);

    /* 2 */
    int s2[] = {1,1,0,1};
    g_callback_count = 0; g_last_event = 0;
    ret = debounce_and_fire(s2, 4, 2, my_press_handler, &invoked);
    printf("2) ret=%d invoked=%d calls=%d\n", ret, invoked, g_callback_count);

    /* 3 */
    int s3[] = {1,1};
    g_callback_count = 0;
    ret = debounce_and_fire(s3, 2, 3, my_press_handler, &invoked);
    printf("3) ret=%d invoked=%d calls=%d\n", ret, invoked, g_callback_count);

    /* 4 */
    int s4[] = {1,1,1,1};
    g_callback_count = 0; g_last_event = 0;
    ret = debounce_and_fire(s4, 4, 4, my_press_handler, &invoked);
    printf("4) ret=%d invoked=%d calls=%d event=%d\n", ret, invoked, g_callback_count, g_last_event);

    /* 5 */
    int s5[] = {0,0,0};
    g_callback_count = 0;
    ret = debounce_and_fire(s5, 3, 1, my_press_handler, &invoked);
    printf("5) ret=%d invoked=%d calls=%d\n", ret, invoked, g_callback_count);

    /* 6: invalid pointers */
    ret = debounce_and_fire(NULL, 3, 1, my_press_handler, &invoked);
    printf("6a) ret=%d\n", ret);

    ret = debounce_and_fire(s5, 3, 1, NULL, &invoked);
    printf("6b) ret=%d\n", ret);

    ret = debounce_and_fire(s5, 3, 1, my_press_handler, NULL);
    printf("6c) ret=%d\n", ret);

    /* 7: invalid sizes */
    ret = debounce_and_fire(s5, -1, 1, my_press_handler, &invoked);
    printf("7a) ret=%d\n", ret);

    ret = debounce_and_fire(s5, 3, 0, my_press_handler, &invoked);
    printf("7b) ret=%d\n", ret);

    return 0;
}
