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
    size_t remaining_space;
    size_t new_record_size;
    
    /* Minimum size for a useful remaining hole: SIZE_T_SIZE + 1 */
    const size_t MIN_HOLE_SIZE = sizeof(size_t) + 1; 

    if (deletedlist == NULL || indexbook == NULL){
        return ERR;
    }

    new_record_size = indexbook_get_size(indexbook);

    pos = deletedlist_find(deletedlist, new_record_size, strategy);
    if (pos == ERR){
        return ERR;
    }
    else if (pos == NO_POS || pos == NOT_FOUND){
        /* No suitable space found, no problem - will append at end */
        return NO_POS;
    }
    
    /* Calculate remaining space: deleted_size - (SIZE_T_SIZE + new_record_size) */
    /* The deleted slot size already includes the original size_t field.
       The new record uses its book_size (R_new) PLUS a new size_t field. */
    if (deletedlist->deleted[pos].register_size < (sizeof(size_t) + new_record_size)){
        /* If the size check in deletedlist_find was correct, this branch is defensive */
        remaining_space = 0; 
    } else {
        remaining_space = deletedlist->deleted[pos].register_size - (sizeof(size_t) + new_record_size);
    }
    
    /* If the remaining space is too small to be a useful hole, remove the entry */
    if (remaining_space < MIN_HOLE_SIZE) {
        for (i = pos; i < deletedlist->used-1; i++){
            deletedlist->deleted[i] = deletedlist->deleted[i+1];
        }
        deletedlist->used--;
        return OK;
    }
    else { 
        /* Update offset to point after the newly written record */
        deletedlist->deleted[pos].offset += sizeof(size_t) + new_record_size;
        deletedlist->deleted[pos].register_size = remaining_space;
        
        /* If using BESTFIT or WORSTFIT, we may need to re-sort this entry */
        if (strategy != FIRSTFIT) {
            IndexDeletedBook temp = deletedlist->deleted[pos];
            
            /* Remove from current position */
            for (i = pos; i < deletedlist->used - 1; i++){
                deletedlist->deleted[i] = deletedlist->deleted[i+1];
            }
            
            /* Find new position based on strategy */
            int new_pos = 0;
            if (strategy == BESTFIT){
                while (new_pos < deletedlist->used - 1 && 
                       deletedlist->deleted[new_pos].register_size < temp.register_size){
                    new_pos++;
                }
            }
            else if (strategy == WORSTFIT){
                while (new_pos < deletedlist->used - 1 && 
                       deletedlist->deleted[new_pos].register_size > temp.register_size){
                    new_pos++;
                }
            }
            
            /* Shift to make room */
            for (i = deletedlist->used - 1; i > new_pos; i--){
                deletedlist->deleted[i] = deletedlist->deleted[i-1];
            }
            
            /* Insert at new position */
            deletedlist->deleted[new_pos] = temp;
        }
        
        return OK;
    }

    return ERR;
}

int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command_code){
    if (deletedlist == NULL || command_code == NO_CMD){
        return ERR;
    }
    
    if(command_code == ADD){
        /* When adding, try to use a deleted space */
        if (deletedlist_del(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    else if(command_code == DEL){
        /* When deleting, add the space to deleted list */
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

    /* We need space for the book_size plus the size_t length field */
    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
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
    
    /* We need space for the book_size plus the size_t length field */
    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
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
        return ERR;
    }
    
    /* We need space for the book_size plus the size_t length field */
    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
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

int deletedlist_save(DeletedList *deletedlist, char *file, int strategy) {
    int k = 0;
    FILE *temp = NULL;

    if (!deletedlist || !deletedlist->deleted || !file) return ERR;

    if (!(temp = fopen(file, "wb"))) return -1;

    if (fwrite(&strategy, sizeof(strategy), 1, temp) != 1) {
        fclose(temp);
        return ERR;
    }

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
    int strategy_header; // Variable to read the sorting strategy header.

    if (!deletedlist_fp) return NULL;

    /* Read the sorting strategy header (an int) */
    if (fread(&strategy_header, sizeof(int), 1, deletedlist_fp) != 1) {
        if (feof(deletedlist_fp)) {
            return deletedlist_init();
        }
        return NULL;
    }

    deletedlist = deletedlist_init();
    if (deletedlist == NULL){
        return NULL;
    }

    while (1) 
    {
        long int offset; 
        size_t register_size;

        if (fread(&offset, sizeof(offset), 1, deletedlist_fp) != 1) {
            if (feof(deletedlist_fp)) break;
            deletedlist_free(deletedlist);
            return NULL;
        }

        if (fread(&register_size, sizeof(register_size), 1, deletedlist_fp) != 1) {
            deletedlist_free(deletedlist);
            return NULL;
        }

        if (deletedlist->size == deletedlist->used) {
            int new_size = deletedlist->size * 2;
            if (new_size == 0) new_size = DELETEDLIST_INIT_SIZE;
            IndexDeletedBook *tmp = realloc(deletedlist->deleted, new_size * sizeof(IndexDeletedBook));
            if (tmp == NULL) {
                deletedlist_free(deletedlist);
                return NULL;
            }
            deletedlist->deleted = tmp;
            deletedlist->size = new_size;
        }

        if (indexdeletedbook_set_offset(&deletedlist->deleted[deletedlist->used], offset) == ERR){
            deletedlist_free(deletedlist);
            return NULL;
        }

        if (indexdeletedbook_set_register_size(&deletedlist->deleted[deletedlist->used], register_size) == ERR){
            deletedlist_free(deletedlist);
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
        fprintf(stdout, "    offset: #%ld\n", deletedList->deleted[i].offset);
        fprintf(stdout, "    size: #%ld\n", deletedList->deleted[i].register_size);        
    }   
}

size_t deletedlist_get_size(DeletedList *deletedlist, int pos)
{
    if (deletedlist == NULL){
        return 0;
    }    

    return deletedlist->deleted[pos].register_size;
}