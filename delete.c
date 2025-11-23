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
    size_t i;

    if (deletedlist == NULL || indexdeletedbook == NULL){
        return -1;
    }

    if (deletedlist->size - deletedlist->used < sizeof(IndexDeletedBook)){
        deletedlist->size += 1;
        deletedlist->deleted = (IndexDeletedBook*)realloc(deletedlist, deletedlist->size);
        if (deletedlist == NULL){
            return -1;
        }
    }

    for(i = 0; i < deletedlist->size-1; i++){
        if (deletedlist->deleted[i] <= deletedlist->deleted[i+1]){
            deletedlist->deleted[deletedlist->size-1] = *indexdeletedbook;
            deletedlist->used ++;
            return 0;
        }
    }

    deletedlist->deleted[deletedlist->size-1] = *indexdeletedbook;
    deletedlist->used ++;
    return 0;

    if (dlist->used == dlist->size){
        int new_size = dlist->size * 2;
        IndexDeletedBook *temp = realloc(dlist->deleted,
                                         new_size * sizeof(IndexDeletedBook));
        if (!temp) return -1;

        dlist->deleted = temp;
        dlist->size = new_size;
    }

    // insertar al final
    dlist->deleted[dlist->used] = *elem;
    dlist->used++;

    return 0;
}
}

int deletedlist_update(DeletedList *deletedlist){
    if (deletedlist == NULL){
        return -1;
    }



}


