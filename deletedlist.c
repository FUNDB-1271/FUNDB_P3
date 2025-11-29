#include "deletedlist.h"

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

    d->size = DELETEDLIST_INIT_SIZE;           
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

int indexdeletedbook_set_register_size(IndexDeletedBook *indexdeletedbook, size_t register_size){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    indexdeletedbook->register_size = register_size;
    return OK;
}

size_t indexdeletedbook_get_register_size(IndexDeletedBook *indexdeletedbook){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    return indexdeletedbook->register_size;
}

int indexdeletedbook_set_offset(IndexDeletedBook *indexdeletedbook, size_t offset){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    indexdeletedbook->offset = offset;
    return OK;
}

size_t indexdeletedbook_get_offset(IndexDeletedBook *indexdeletedbook){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    return indexdeletedbook->offset;
}

int deletedlist_add(DeletedList *deletedlist, IndexBook *indexbook, int strategy){
    int i, j;

    IndexDeletedBook deletedbook;

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

    pos = deletedlist_find(deletedlist, indexbook_get_size(indexbook), strategy);
    if (pos == ERR || pos == NO_POS){
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


int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command_code){
    if (deletedlist == NULL || command_code == NO_CMD){
        return -1;
    }
    
    if(command_code == ADD){
        if (deletedlist_del(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    else if(command_code == DEL){
        if (deletedlist_add(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    
    return OK;
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
        return ERR;
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

    return ERR;
}

int deletedlist_save(DeletedList *deletedlist, char *file) {
    int k = 0;
    FILE *temp = NULL;

    if (!deletedlist || !deletedlist->deleted || !file) return ERR;

    /* create temporary file to ensure either all of the index is written or the file stays the same */
    if (!(temp = fopen(file, "wb"))) return -1;

    /* WRITE A HEADER FOR THE INDEX FILE (?) */

    for (k = 0 ; k < deletedlist->used ; k++) {
        if (fwrite(&(deletedlist->deleted[k].offset), sizeof(deletedlist->deleted[k].offset), 1, temp) != 1){
            fclose (temp);
            return ERR;
        }
        if (fwrite(&(deletedlist->deleted[k].register_size), sizeof(deletedlist->deleted[k].register_size), 1, temp) != 1){
            fclose (temp);
            return ERR;
        }
    }
    
    fflush(temp);
    fclose(temp);

    return OK;
}

DeletedList *deletedlist_init_from_file(FILE *deletedlist_fp) {
    DeletedList *deletedlist = NULL;

    if (!deletedlist_fp) return NULL;

    deletedlist = deletedlist_init();
    if (deletedlist == NULL){
        return NULL;
    }      

    while (1) 
    {
        long int offset; 
        size_t register_size;

        /* expandir array */
        if (deletedlist->size == deletedlist->used) {
            deletedlist->size *= 2;
            deletedlist->deleted = realloc(deletedlist->deleted, deletedlist->size * sizeof(IndexBook *));
            if (deletedlist->deleted){
                if (deletedlist) deletedlist_free(deletedlist);
                return NULL;
            }
        }

        /* lectura binaria */
        if (fread(&offset, sizeof(offset), 1, deletedlist_fp) != 1){
            if (deletedlist_fp) fclose(deletedlist_fp);
            if (deletedlist) deletedlist_free(deletedlist);
            break;
        }
        if (fread(&register_size, sizeof(register_size), 1, deletedlist_fp) != 1){
            if (deletedlist_fp) fclose(deletedlist_fp);
            if (deletedlist) deletedlist_free(deletedlist);
        }

        if (indexdeletedbook_set_offset(&deletedlist->deleted[deletedlist->used], offset) == ERR){
            if (deletedlist_fp) fclose(deletedlist_fp);
            if (deletedlist) deletedlist_free(deletedlist);
            return NULL;
        }

        if (indexdeletedbook_set_register_size(&deletedlist->deleted[deletedlist->used], register_size) == ERR){
            if (deletedlist_fp) fclose(deletedlist_fp);
            if (deletedlist) deletedlist_free(deletedlist);
            return NULL;
        }
        
        deletedlist->used++;
    }

    return deletedlist;
}

void deletedlist_print(DeletedList *deletedList) {
    int i;
    if (deletedList == NULL || deletedList->deleted == NULL) return;
    for (i = 0 ; i < deletedList->used ; i++) {
        fprintf(stdout, "Entry #%d\n", i);
        fprintf(stdout, "\toffset: #%ld\n", deletedList->deleted[i].offset);
        fprintf(stdout, "\tsize: #%ld\n", deletedList->deleted[i].register_size);        
    }   
}