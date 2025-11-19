#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* used for memory-related functions */

struct _indexbook {
    int key; 
    long int offset;
    size_t size;
};

struct _index {
    IndexBook *books;
    size_t used;
    size_t size;
};

Index *index_init(void) {
    Index *ind = malloc(sizeof(Index));
    if (ind != NULL) {
        ind->books = (IndexBook *) malloc(INDEX_INIT_SIZE * sizeof(IndexBook));
        if (ind->books == NULL){
            free(ind);
            return NULL;
        }
        ind->size = INDEX_INIT_SIZE;
        ind->used = 0;
    }

    return ind;
}

void index_free(Index *ind) {
    if (ind != NULL) 
    {
        if (ind->books != NULL){
            free(ind->books);
        } 
        ind->books = NULL;
        free(ind);
    }
}

int index_add(Index *ind, IndexBook *book) {
    int m, a = 0, b = ind->used - 1;

    if (ind->size == ind->used) /* ensure at least one free space */
    {
        ind->size *= 2;
        ind->books = realloc(ind->books, ind->size * sizeof(IndexBook));
        if (!ind->books){
            ind->size = 0; 
            return - 1;
        } 
    }

    while (a <= b) 
    {
        m = (a + (b - a)) / 2; /* minimize overflow */

        if (book->key == ind->books[m].key) {
            return;
        } else if (book->key < ind->books[m].key) {
            b = m - 1;
        } else {
            a = m + 1;
        } 
    }

    if (ind->used - a > 0) 
    {
        memmove(&ind->books[a+1], &ind->books[a], (ind->used - a) * sizeof(IndexBook));
    }

    ind->books[a] = *book;
    ind->used++;

    return 0;
}


int index_save(Index *ind, char *file) {
    ssize_t k = 0;
    int check = 0;
    FILE *temp = NULL;

    if (!ind || !ind->books || !file) return -1;

    if (!(temp = fopen("save.temp", "wb"))) return -1;

    /* WRITE A HEADER FOR THE INDEX FILE (?) */

    for (k = 0 ; k < ind->used ; k++) {
        if (fwrite(&(ind->books[k].key), sizeof(ind->books->key), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k].offset), sizeof(ind->books->offset), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k].size), sizeof(ind->books->size), 1, temp) != 1) goto write_fail;
    }
    
    fflush(temp);
    fclose(temp);
    rename("save.temp", file);

    return 0;

write_fail:
    {
        
    }
    return -1;
}
