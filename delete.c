#include "delete.h"

#include <stdio.h>
#include <stdlib.h>

#define NO_POS -2

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

size_t deletedlist_get_offset(DeletedList *deletedlist, int pos){
    if (deletedlist == NULL){
        return 0;
    }    

    return deletedlist->deleted[pos].offset;
}

int deletedlist_add(DeletedList *deletedlist, IndexBook *indexbook, int strategy){
    int i, j;

    IndexDeletedBook deletedbook, temp;

    deletedbook.register_size = indexbook_get_size(indexbook);
    deletedbook.offset = indexbook_get_offset(indexbook);

    if (deletedlist == NULL || indexbook == NULL){
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

    if (strategy == FIRSTFIT){
        deletedlist->deleted[deletedlist->used++] = deletedbook;
        return OK;  
    }
    else if(strategy == BESTFIT){
        i = 0;
        while (i < deletedlist->used && deletedlist->deleted[i].register_size < deletedbook.register_size){
            i++;
        }
    }
    else if(strategy == WORSTFIT){
        i = 0;
        while (i < deletedlist->used && deletedlist->deleted[i].register_size > deletedbook.register_size){
            i++;
        }        
    }
    else{
        return ERR;
    }

    for (j = deletedlist->used; j > i; j--) {
        deletedlist->deleted[j] = deletedlist->deleted[j-1];
    }

    deletedlist->deleted[i] = deletedbook;
    deletedlist->used++;
    
    return OK;
}

int deletedlist_del(DeletedList *deletedlist, IndexBook *indexbook, int strategy){
    int i, pos;

    if (deletedlist == NULL){
        return ERR;
    }

    if (strategy == BESTFIT){
        pos = deletedlist_findbestfit(deletedlist, indexbook_get_size(indexbook));
        if (pos == ERR || pos == NO_POS){
            return ERR;
        }
    }
    else if(strategy == WORSTFIT){
        pos = deletedlist_findworstfit(deletedlist, indexbook_get_size(indexbook));
        if (pos == ERR || pos == NO_POS){
            return ERR;
        }
    }
    else if (strategy == FIRSTFIT){
        pos = deletedlist_findfirstfit(deletedlist, indexbook_get_size(indexbook));
        if (pos == ERR || pos == NO_POS){
            return ERR;
        }
    }
    else{
        return ERR;
    }

    if (deletedlist->deleted[pos].register_size == indexbook_get_size(indexbook)){
        for (i = pos; i < deletedlist->used-1; i++){
            deletedlist->deleted[i] = deletedlist->deleted[i+1];
        }

        deletedlist->used--;
        return OK;
    }
    else if (deletedlist->deleted[pos].register_size > indexbook_get_size(indexbook)){
        deletedlist->deleted[pos].register_size -= indexbook_get_size(indexbook);

        return OK;
    }

    return ERR;
}


int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command){
    if (deletedlist == NULL || command == NO_CMD){
        return -1;
    }

    int pos;
    
    if(command == ADD){
        if (deletedlist_del(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    else if(command == DEL){
        if (deletedlist_add(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    

    return 0;
}

int deletedlist_findbestfit(DeletedList *deletedlist, size_t book_size){
    int i, pos = NO_POS;
    size_t min = 0;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return ERR;
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
    int i, pos = NO_POS;
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

    return NO_POS;
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