
#include <stdio.h>
#include <stdbool.h>

static void process_char(unsigned char c,
                         bool *line_started,
                         bool *saw_non_ws,
                         bool *first_non_space_set,
                         char *first_non_space) {
    *line_started = true;

    if (c != ' ' && c != '\t') {
        *saw_non_ws = true;
        if (!*first_non_space_set) {
            *first_non_space_set = true;
            *first_non_space = (char)c;
        }
    }
}

static void finalize_line(bool ignore_blank,
                          bool ignore_comment_hash,
                          bool *line_started,
                          bool *saw_non_ws,
                          bool *first_non_space_set,
                          char *first_non_space,
                          int *count) {
    bool is_blank = !(*saw_non_ws);
    bool is_comment = (*first_non_space_set && *first_non_space == '#');

    if (!(ignore_blank && is_blank) &&
        !(ignore_comment_hash && is_comment)) {
        (*count)++;
    }

    *line_started = false;
    *saw_non_ws = false;
    *first_non_space_set = false;
    *first_non_space = '\0';
}

int count_lines_filtered(const char *path,
                         bool ignore_blank,
                         bool ignore_comment_hash,
                         int *out_count) {
    FILE *f;
    unsigned char buf[4096];
    size_t nread, i;
    int count = 0;

    bool line_started = false;
    bool saw_non_ws = false;
    bool first_non_space_set = false;
    char first_non_space = '\0';
    bool pending_cr = false;

    if (path == NULL || out_count == NULL) return -1;

    f = fopen(path, "rb");
    if (f == NULL) return -1;

    while ((nread = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (i = 0; i < nread; i++) {
            unsigned char b = buf[i];

            if (pending_cr) {
                if (b == '\n') {
                    pending_cr = false;
                    finalize_line(ignore_blank, ignore_comment_hash,
                                  &line_started, &saw_non_ws,
                                  &first_non_space_set, &first_non_space,
                                  &count);
                    continue;
                } else {
                    process_char('\r', &line_started, &saw_non_ws,
                                 &first_non_space_set, &first_non_space);
                    pending_cr = false;
                }
            }

            if (b == '\r') {
                pending_cr = true; /* may be stripped if next is '\n' */
            } else if (b == '\n') {
                finalize_line(ignore_blank, ignore_comment_hash,
                              &line_started, &saw_non_ws,
                              &first_non_space_set, &first_non_space,
                              &count);
            } else {
                process_char(b, &line_started, &saw_non_ws,
                             &first_non_space_set, &first_non_space);
            }
        }
    }

    if (ferror(f)) {
        fclose(f);
        return -1;
    }

    if (pending_cr) {
        process_char('\r', &line_started, &saw_non_ws,
                     &first_non_space_set, &first_non_space);
        pending_cr = false;
    }

    if (line_started) {
        finalize_line(ignore_blank, ignore_comment_hash,
                      &line_started, &saw_non_ws,
                      &first_non_space_set, &first_non_space,
                      &count);
    }

    if (fclose(f) != 0) return -1;

    *out_count = count;
    return 0;
}

/* quick demo */
int main(void) {
    int c, ret;

    ret = count_lines_filtered("test1.txt", false, false, &c);
    printf("test1: ret=%d count=%d\n", ret, c);

    ret = count_lines_filtered("test2.txt", true, true, &c);
    printf("test2: ret=%d count=%d\n", ret, c);

    return 0;
}
