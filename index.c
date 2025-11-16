#include "index.h"

#include <stdio.h>
#include <stdlib.h>

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

void index_init(Index *ind) {
    ind->books = (IndexBook *) malloc(INDEX_INIT_SIZE * sizeof(IndexBook));
    ind->size = INDEX_INIT_SIZE;
    ind->used = 0;
}

void index_free(Index *ind) {
    if (ind) 
    {
        if (ind->books) free(ind->books);
        ind->size = 0;
        ind->used = 0;
    }
}

void index_add(Index *ind, IndexBook *book) {
    /* aquí va una búsqueda binaria para insertar el elemento en su posición correspondiente en el índice */

}



void index_save();