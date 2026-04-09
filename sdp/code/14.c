
#include <stdio.h>
#include <stdbool.h>

int safe_copy_label(const char *src,
                    char *dst,
                    int dst_capacity,
                    bool to_lower,
                    int *out_copied,
                    bool *out_truncated) {
    int i = 0;
    int limit;

    /* Validate first (no output modification on error) */
    if (src == NULL || dst == NULL || out_copied == NULL || out_truncated == NULL) {
        return -1;
    }
    if (dst_capacity < 0) {
        return -1;
    }

    limit = (dst_capacity > 0) ? (dst_capacity - 1) : 0;

    /* Copy up to limit or until '\0' */
    while (src[i] != '\0' && i < limit) {
        char ch = src[i];
        if (to_lower && ch >= 'A' && ch <= 'Z') {
            ch = (char)(ch + ('a' - 'A'));
        }
        dst[i] = ch;
        i++;
    }

    /* Null-terminate if possible */
    if (dst_capacity > 0) {
        dst[i] = '\0';
    }

    /* Outputs */
    *out_copied = i;
    *out_truncated = (src[i] != '\0'); /* source still has chars => truncated */

    return 0;
}

/* ---------- tests ---------- */
int main(void) {
    char dst[32];
    int copied;
    bool truncated;
    int ret;

    ret = safe_copy_label("Hello", dst, 10, false, &copied, &truncated);
    printf("1) ret=%d copied=%d truncated=%d dst=\"%s\"\n", ret, copied, truncated, dst);

    ret = safe_copy_label("Hello", dst, 6, true, &copied, &truncated);
    printf("2) ret=%d copied=%d truncated=%d dst=\"%s\"\n", ret, copied, truncated, dst);

    ret = safe_copy_label("FirmwareV1", dst, 5, false, &copied, &truncated);
    printf("3) ret=%d copied=%d truncated=%d dst=\"%s\"\n", ret, copied, truncated, dst);

    ret = safe_copy_label("", dst, 3, true, &copied, &truncated);
    printf("4) ret=%d copied=%d truncated=%d dst=\"%s\"\n", ret, copied, truncated, dst);

    ret = safe_copy_label("ABC", dst, 0, false, &copied, &truncated);
    printf("5) ret=%d copied=%d truncated=%d (dst not written)\n", ret, copied, truncated);

    ret = safe_copy_label(NULL, dst, 5, false, &copied, &truncated);
    printf("6a) ret=%d\n", ret);

    ret = safe_copy_label("Hello", NULL, 5, false, &copied, &truncated);
    printf("6b) ret=%d\n", ret);

    ret = safe_copy_label("Hello", dst, -1, false, &copied, &truncated);
    printf("7) ret=%d\n", ret);

    return 0;
}
