#include "midiff.h"

void usage(void) {
        fputs("usage: midiff [-c context_lines] [-m max_lines] file1 file2\n", stderr);

        exit(EXIT_SUCCESS);
}

void die(const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        fprintf(stderr, "FATAL ERROR: ");
        vfprintf(stderr, fmt, vargs);

        fputc('\n', stderr);

        va_end(vargs);

        exit(EXIT_FAILURE);
}
