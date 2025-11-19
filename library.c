#include "commands.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BEST_FIT "best_fit"
#define WORST_FIT "worst_fit"
#define FIRST_FIT "first_fit"

void _library_init(char *dbname, FILE **db, FILE **ind, FILE **lst, DeletedList **dellst, Index **index);

void _library_cleanup(FILE **db, FILE **ind, FILE **lst, DeletedList **dellst, Index **index);

void loop();

int main(int argc, char *argv[]) {
    DeletedList *lst = NULL;
    Index *ind = NULL;
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
    

    _library_init(argv[2], &data_file, &index_file, &deleted_file, &lst, &ind);

    loop(/* ... */);

    _library_cleanup(&data_file, &index_file, &deleted_file, &lst, &ind);

    return 0;
}

void _library_init(char *dbname, FILE **db, FILE **ind, FILE **lst, DeletedList **dellst, Index **index) {
    char dataf_name[32];
    char indexf_name[32];
    char deletedf_name[32];
    
    if (!dbname || !db || !ind || !lst || !dellst || !index || srlen(dbname) > 31) return;

    snprintf(dataf_name, sizeof(dataf_name), "%s.db", dbname);
    snprintf(indexf_name, sizeof(indexf_name), "%s.ind", dbname);
    snprintf(deletedf_name, sizeof(deletedf_name), "%s.lst", dbname);

    if (!(*db = fopen(dataf_name, "wb+"))) 
    {
        fprintf(stdout, "Error opening file %s\n", dataf_name);
        return;
    }

    if (!(*ind = fopen(indexf_name, "wb+"))) 
    {
        fprintf(stdout, "Error opening file %s\n", indexf_name);
        fclose(*db);
        return;
    }

    if (!(*lst = fopen(deletedf_name, "wb+"))) 
    {
        fprintf(stdout, "Error opening file %s\n", deletedf_name);
        fclose(*db);
        fclose(*ind);
        return;
    }

    if (!(*index = index_init()))
    {
        fprintf(stdout, "Error allocating deleted list structure\n");
        fclose(*db);
        fclose(*ind);
        fclose(*lst);
        return;
    }

    if (!(*dellst = deletedlist_init()))
    {
        fprintf(stdout, "Error allocating index structure\n");
        index_free(*ind);
        fclose(*db);
        fclose(*ind);
        fclose(*lst);
        return;
    }
}

void loop() {

}

void _library_cleanup(FILE **db, FILE **ind, FILE **lst, DeletedList **dellst, Index **index) {
    if (*db) fclose(*db);
    if (*ind) fclose(*ind);
    if (*lst) fclose(*lst);
    if (*dellst) deletedlist_free(*dellst);
    if (*index) index_free(*index);
}
