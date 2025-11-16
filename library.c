#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BEST_FIT "best_fit"
#define WORST_FIT "worst_fit"
#define FIRST_FIT "first_fit"

void _library_init();

void _library_cleanup();

int main(int argc, char *argv[]) {

    /* no tocar este mensaje */
    if (argc != 3) {
        fprintf(stdout, "Missing argument");
        return 0;
    }

    /* no tocar este mensaje */
    if (strcmp(argv[1], BEST_FIT) != 0 && strcmp(argv[1], BEST_FIT) != 0 && strcmp(argv[1], BEST_FIT) != 0) {
        fprintf(stdout, "Unknown search strategy %s", argv[1]);
        return 0;
    }

    _library_init(/* ... */);

    loop(/* ... */);

    _library_free(/* ... */);

    return 0;
}
