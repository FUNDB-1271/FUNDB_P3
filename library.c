#include "commands.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BEST_FIT "best_fit"
#define WORST_FIT "worst_fit"
#define FIRST_FIT "first_fit"

void _library_init();

void _library_cleanup();

void loop();

int main(int argc, char *argv[]) {
    FILE *data_file = NULL, *index_file = NULL, *deleted_file = NULL;
    char dataf_name[32];
    char indexf_name[32];
    char deletedf_name[32];

    /* no tocar este mensaje */
    if (argc != 3) {
        fprintf(stdout, "Missing argument");
        return 0;
    }

    /* no tocar este mensaje */
    if (strcmp(argv[1], BEST_FIT) != 0 && strcmp(argv[1], WORST_FIT) != 0 && strcmp(argv[1], FIRST_FIT) != 0) {
        fprintf(stdout, "Unknown search strategy %s", argv[1]);
        return 0;
    }
    
    snprintf(dataf_name, sizeof(dataf_name), "%s.db", argv[2]);
    snprintf(indexf_name, sizeof(indexf_name), "%s.ind", argv[2]);
    snprintf(deletedf_name, sizeof(deletedf_name), "%s.lst", argv[2]);

    /* IN REALITY THESE FILE INITIALIZATIONS WILL HAPPEN IN LIBRARY INIT */

    /* revise these file permissions */
    if (!(data_file = fopen(dataf_name, "ab+"))) 
    {
        fprintf(stdout, "Error opening file %s\n", dataf_name);
        return 1;
    }

    if (!(index_file = fopen(indexf_name, "ab+"))) 
    {
        fprintf(stdout, "Error opening file %s\n", indexf_name);
        fclose(data_file);
        return 1;
    }

    if (!(deleted_file = fopen(deletedf_name, "w"))) 
    {
        fprintf(stdout, "Error opening file %s\n", deletedf_name);
        fclose(data_file);
        fclose(index_file);
        return 1;
    }

    _library_init(/* ... */);

    loop(/* ... */);

    _library_cleanup(/* ... */);

    return 0;
}

void loop() {}

void _library_init() {}

void _library_cleanup() {}
