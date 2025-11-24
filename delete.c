#include "delete.h"

#include <stdio.h>
#include <stdlib.h>

struct _indexdeletedbook {
    size_t register_size;
    size_t offset;
};

struct _deletedlist {
    IndexDeletedBook *deleted;
    int used;
    int size;
};

DeletedList *deletedlist_init() {
    DeletedList *d = malloc(sizeof(DeletedList));
    if (d == NULL){
        return NULL;
    }

    d->size = 4;           
    d->used = 0;   

    d->deleted = malloc(d->size * sizeof(IndexDeletedBook));
    if (d->deleted == NULL){
        free(d);
        return NULL;
    }

    return d;
}

void deletedlist_free(DeletedList *deletedlist){
    if (deletedlist != NULL){
        if (deletedlist->deleted != NULL){
            free(deletedlist->deleted);
        }
        free(deletedlist);
    }
}

int deletedlist_add(DeletedList *deletedlist, IndexDeletedBook *indexdeletedbook){
    int i, j;
    IndexDeletedBook temp;

    if (deletedlist == NULL || indexdeletedbook == NULL){
        return -1;
    }

    if (deletedlist->size == deletedlist->used){
        deletedlist->size *= 2;
        if (deletedlist->size == 0){
            deletedlist->size = 1;
        }
        deletedlist->deleted = realloc(deletedlist->deleted, deletedlist->size * sizeof(IndexDeletedBook));
        if (deletedlist->deleted == NULL){
            return -1;
        }
    }
    else if(deletedlist->size < deletedlist->used){
        return -1;
    }

    /*Insertamos en la posición correcta el indexdeletedbook*/
    for (i = 0; i < deletedlist->used; i++) {
    if (indexdeletedbook->register_size < deletedlist->deleted[i].register_size) {
        for (j = deletedlist->used; j > i; j--) {
            deletedlist->deleted[j] = deletedlist->deleted[j-1];
        }
        deletedlist->deleted[i] = *indexdeletedbook;
        deletedlist->used++;
        return 0;
    }
    }

    deletedlist->deleted[deletedlist->used] = *indexdeletedbook;
    deletedlist->used++;


    return 0;
}


int deletedlist_update(DeletedList *deletedlist){
    if (deletedlist == NULL){
        return -1;
    }



}


