
#include <stdio.h>

int find_subsequence_in_slice(const char *buf,
                              int n,
                              const char *needle,
                              int m,
                              char **out_ptr,
                              int *out_offset) {
    const char *end;
    const char *needle_end;
    const char *p;

    /* Validate: no output changes on error */
    if (buf == NULL || needle == NULL || out_ptr == NULL || out_offset == NULL) return -1;
    if (n < 0 || m <= 0) return -1;

    end = buf + n;
    needle_end = needle + m;

    for (p = buf; p + m <= end; ++p) {
        const char *q = p;
        const char *r = needle;

        while (r < needle_end && *q == *r) {
            ++q;
            ++r;
        }

        if (r == needle_end) {
            *out_ptr = (char *)p;               /* match found */
            *out_offset = (int)(p - buf);
            return 0;
        }
    }

    *out_ptr = NULL;                            /* not found */
    *out_offset = -1;
    return 0;
}

/* ---------------- tests ---------------- */
int main(void) {
    int ret, off;
    char *ptr;

    {
        const char buf[] = "ABCDEF";
        const char ndl[] = "CDE";
        ret = find_subsequence_in_slice(buf, 6, ndl, 3, &ptr, &off);
        printf("1) ret=%d off=%d found=%s\n", ret, off, (ptr ? "yes" : "no"));
    }

    {
        const char buf[] = "AAAAA";
        const char ndl[] = "AAA";
        ret = find_subsequence_in_slice(buf, 5, ndl, 3, &ptr, &off);
        printf("2) ret=%d off=%d found=%s\n", ret, off, (ptr ? "yes" : "no"));
    }

    {
        const char buf[] = "ABCDE";
        const char ndl[] = "DEF";
        ret = find_subsequence_in_slice(buf, 5, ndl, 3, &ptr, &off);
        printf("3) ret=%d off=%d found=%s\n", ret, off, (ptr ? "yes" : "no"));
    }

    {
        const char buf[] = "XYZ";
        const char ndl[] = "XYZ";
        ret = find_subsequence_in_slice(buf, 3, ndl, 3, &ptr, &off);
        printf("4) ret=%d off=%d found=%s\n", ret, off, (ptr ? "yes" : "no"));
    }

    {
        const char buf[] = "HI";
        const char ndl[] = "HIK";
        ret = find_subsequence_in_slice(buf, 2, ndl, 3, &ptr, &off);
        printf("5) ret=%d off=%d found=%s\n", ret, off, (ptr ? "yes" : "no"));
    }

    ret = find_subsequence_in_slice(NULL, 5, "A", 1, &ptr, &off);
    printf("6a) ret=%d\n", ret);

    ret = find_subsequence_in_slice("ABCDE", 5, NULL, 1, &ptr, &off);
    printf("6b) ret=%d\n", ret);

    ret = find_subsequence_in_slice("ABCDE", -1, "A", 1, &ptr, &off);
    printf("7a) ret=%d\n", ret);

    ret = find_subsequence_in_slice("ABCDE", 5, "A", 0, &ptr, &off);
    printf("7b) ret=%d\n", ret);

    return 0;
}
