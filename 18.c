
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int dynint_append(int **buf, int *count, int *capacity, int value) {
    int old_count, old_capacity;
    int *old_buf;
    int needed;
    int new_capacity;
    long max_capacity;
    long bytes;
    int *tmp;

    if (buf == NULL || count == NULL || capacity == NULL) return -1;

    old_buf = *buf;
    old_count = *count;
    old_capacity = *capacity;

    if (old_count < 0 || old_capacity < 0) return -1;
    if (old_count > old_capacity) return -1;
    if (old_capacity == 0 && old_buf != NULL) return -1;
    if (old_capacity > 0 && old_buf == NULL) return -1;

    if (old_count < old_capacity) {
        old_buf[old_count] = value;
        *count = old_count + 1;
        return 0;
    }

    needed = old_count + 1;
    max_capacity = (long)INT_MAX / (long)sizeof(int);

    if (old_capacity == 0) {
        new_capacity = 4;
    } else {
        if ((long)old_capacity <= max_capacity / 2) {
            new_capacity = old_capacity * 2;
        } else {
            new_capacity = (int)max_capacity;
        }
    }

    if ((long)new_capacity < (long)needed || (long)new_capacity > max_capacity) {
        return -3; /* overflow / cannot grow safely */
    }

    bytes = (long)new_capacity * (long)sizeof(int);

    if (old_capacity == 0) {
        tmp = (int *)malloc((size_t)bytes);
    } else {
        tmp = (int *)realloc(old_buf, (size_t)bytes);
    }

    if (tmp == NULL) {
        return -2; /* allocation failure, state unchanged */
    }

    *buf = tmp;
    *capacity = new_capacity;
    (*buf)[old_count] = value;
    *count = old_count + 1;

    return 0;
}

/* ---------------- tests ---------------- */
static void print_buf(const int *buf, int count, int capacity) {
    int i;
    printf("count=%d cap=%d data=[", count, capacity);
    for (i = 0; i < count; i++) {
        printf("%d", buf[i]);
        if (i + 1 < count) printf(",");
    }
    printf("]\n");
}

int main(void) {
    int *buf = NULL;
    int count = 0;
    int capacity = 0;
    int ret;

    /* 1 */
    ret = dynint_append(&buf, &count, &capacity, 10);
    printf("1) ret=%d ", ret); print_buf(buf, count, capacity);

    /* 2 */
    ret = dynint_append(&buf, &count, &capacity, 20);
    ret = dynint_append(&buf, &count, &capacity, 30);
    ret = dynint_append(&buf, &count, &capacity, 40);
    printf("2) ret=%d ", ret); print_buf(buf, count, capacity);

    /* 3 */
    ret = dynint_append(&buf, &count, &capacity, 50);
    printf("3) ret=%d ", ret); print_buf(buf, count, capacity);

    /* 4: invalid invariants */
    {
        int *b2 = NULL;
        int c2 = -1, cap2 = 0;
        ret = dynint_append(&b2, &c2, &cap2, 1);
        printf("4) ret=%d\n", ret); /* -1 */
    }

    /* 5: overflow path simulation */
    {
        int *b3 = buf; /* any non-NULL pointer */
        int c3 = INT_MAX / (int)sizeof(int);
        int cap3 = INT_MAX / (int)sizeof(int);
        ret = dynint_append(&b3, &c3, &cap3, 77);
        printf("5) ret=%d\n", ret); /* -3 */
    }

    free(buf);
    return 0;
}