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
            deletedlist->size = 10;
        }
        deletedlist->deleted = realloc(deletedlist->deleted, deletedlist->size * sizeof(IndexDeletedBook));
        if (deletedlist->deleted == NULL){
            return -1;
        }
    }
    else if(deletedlist->size < deletedlist->used){
        return -1;
    }

    deletedlist->deleted[deletedlist->used] = *indexdeletedbook;
    deletedlist->used++;

    return 0;
}

int deletedlist_del(DeletedList *deletedlist, int pos){
    int i;

    if (deletedlist == NULL || pos > deletedlist->used || pos < 0){
        return -1;
    }

    
    for (i = pos; i < deletedlist->used-1; i++){
        deletedlist->deleted[i] = deletedlist->deleted[i+1];
    }

    deletedlist->used--;

    return 0;
}


int deletedlist_update(DeletedList *deletedlist, size_t book_size, int strategy){
    if (deletedlist == NULL || book_size == 0){
        return -1;
    }

    int pos;

    if (strategy == BESTFIT){
        pos = deletedlist_findbestfit(deletedlist, book_size);
        if (pos == -1){
            return -1;
        }
    }
    else if(strategy == WORSTFIT){
        pos = deletedlist_findworstfit(deletedlist, book_size);
        if (pos == -1){
            return -1;
        }
    }
    else if (strategy == FIRSTFIT){
        pos = deletedlist_findfirstfit(deletedlist, book_size);
        if (pos == -1){
            return -1;
        }
    }
    else{
        return -1;
    }
    

    if (deletedlist->deleted[pos].register_size == book_size){
        if (deletedlist_del(deletedlist, pos) == -1){
            return -1;
        }
    }
    else if (deletedlist->deleted[pos].register_size > book_size){
        deletedlist->deleted[pos].register_size -= book_size;
    }
    else{
        return -1;
    }

    return 0;
}

int deletedlist_findbestfit(DeletedList *deletedlist, size_t book_size){
    int i, pos = -1;
    size_t min = 0;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return -1;
    }

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= book_size){
            if (min > deletedlist->deleted[i].register_size || min == 0){
                pos = i;
                min = deletedlist->deleted[i].register_size;
            }
        }
    }

    return pos;
}

int deletedlist_findworstfit(DeletedList *deletedlist, size_t book_size){
    int i, pos = -1;
    size_t max = 0;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return -1;
    }

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= book_size){
            if (max < deletedlist->deleted[i].register_size || max == 0){
                pos = i;
                max = deletedlist->deleted[i].register_size;
            }
        }
    }

    return pos;
}

int deletedlist_findfirstfit(DeletedList *deletedlist, size_t book_size){
    int i;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return -1;
    }

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= book_size){
            return i;
        }
    }

    return -1;
}   

int deletedlist_find(DeletedList *deletedlist, size_t book_size, int strategy){
    if (deletedlist == NULL || (strategy != FIRSTFIT && strategy != WORSTFIT && strategy != BESTFIT)){
        return -1;
    }

    if (strategy == BESTFIT){
        return deletedlist_findbestfit(deletedlist, book_size);
    }
    else if (strategy == WORSTFIT){
        return deletedlist_findworstfit(deletedlist, book_size);
    }
    else if (strategy == FIRSTFIT){ 
        return deletedlist_findfirstfit(deletedlist, book_size);
    }
}


void deletedlist_print(DeletedList *deletedList) {
    int i;
    if (deletedList == NULL || deletedList->deleted == NULL) return;
    for (i = 0 ; i < deletedList->used ; i++) {
        fprintf(stdout, "Entry #%ld\n", i);
        fprintf(stdout, "\toffset: #%d\n", deletedList->deleted[i].offset);
        fprintf(stdout, "\tsize: #%ld\n", deletedList->deleted[i].register_size);        
    }   
}