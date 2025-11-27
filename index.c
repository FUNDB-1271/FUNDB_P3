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
    IndexBook **books; /* pointers to books */
    size_t used;
    size_t size;
};



IndexBook *indexbook_init(int key, long int offset, size_t size) {
    IndexBook *book = (IndexBook *) malloc(sizeof(IndexBook));
    if (book) {
        book->key = key;
        book->offset = offset;
        book->size = size;
    }
    return book;
}


void indexbook_free(IndexBook *book) {
    if (book) free(book);
}

size_t indexbook_get_size(IndexBook *book){
    if (book == NULL){
        return 0;
    }

    return book->size;
}

int indexbook_get_key(IndexBook *book){
    return book->key;
}

int indexbook_set_offset(IndexBook *book, size_t offset){
    if (book == NULL){
        return ERR;
    }

    book->offset = offset;

    return OK;
}

long int indexbook_get_offset(IndexBook *book){
    if (book == NULL){
        return ERR;
    }

    return book->offset;
}

Index *index_init(void) {
    Index *ind = malloc(sizeof(Index));
    if (ind) {
        ind->books = (IndexBook **) malloc(INDEX_INIT_SIZE * sizeof(IndexBook *));
        if (!ind->books) {
            free(ind);
            return NULL;
        }
        ind->size = INDEX_INIT_SIZE;
        ind->used = 0;
    }

    return ind;
}

void index_free(Index *ind) {
    size_t i;
    if (ind) 
    {
        if (ind->books) {
            for (i = 0 ; i < ind->used ; i++) if (ind->books[i]) indexbook_free(ind->books[i]);
            free(ind->books);
        }
        ind->books = NULL;
        free(ind);
    }
}

int index_add(Index *ind, IndexBook *book) {
    int m, a = 0, b = ind->used - 1;

    if (!ind || !ind->books || !book) return -1;

    if (ind->size == ind->used) /* ensure at least one free space */
    {
        size_t new_size = ind->size * 2;
        IndexBook **temp = realloc(ind->books, new_size * sizeof(IndexBook *));
        if (!temp) {
            return -1;
        }
        ind->books = temp;
        ind->size = new_size;
    }

    while (a <= b) 
    {
        m = a + (b - a) / 2; /* minimize overflow */

        if (book->key == ind->books[m]->key) {
            return 1;
        } else if (book->key < ind->books[m]->key) {
            b = m - 1;
        } else {
            a = m + 1;
        } 
    }

    if (ind->used - a > 0) 
    {
        memmove(&(ind->books[a+1]), &(ind->books[a]), (ind->used - a) * sizeof(IndexBook *));
    }

    ind->books[a] = book;
    ind->used++;

    return 0;
}

int index_save(Index *ind, char *file) {
    size_t k = 0;
    FILE *temp = NULL;

    if (!ind || !ind->books || !file) return -1;

    /* create temporary file to ensure either all of the index is written or the file stays the same */
    if (!(temp = fopen("save.temp", "wb"))) return -1;

    /* WRITE A HEADER FOR THE INDEX FILE (?) */

    for (k = 0 ; k < ind->used ; k++) {
        if (fwrite(&(ind->books[k]->key), sizeof(ind->books[k]->key), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k]->offset), sizeof(ind->books[k]->offset), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k]->size), sizeof(ind->books[k]->size), 1, temp) != 1) goto write_fail;
    }
    
    fflush(temp);
    fclose(temp);
    rename("save.temp", file);

    return 0;

write_fail:
    {
        fclose(temp);
    }
    return -1;
}

Index *index_init_from_file(FILE *index_fp) {
    Index *index = NULL;
    IndexBook *book = NULL;

    if (!index_fp) return NULL;

    if (!(index = (Index *) malloc(sizeof(Index)))) goto cleanup;

    if (!(index->books = (IndexBook **) malloc(INDEX_INIT_SIZE * sizeof(IndexBook *)))) goto cleanup;

    index->size = INDEX_INIT_SIZE;
    index->used = 0;         

    while (1) 
    {
        book = NULL;
        long offset; 
        size_t size;
        int key;

        /* expandir array */
        if (index->size == index->used) {
            index->size *= 2;
            index->books = realloc(index->books, index->size * sizeof(IndexBook *));
            if (!(index->books)) goto cleanup;
        }

        /* lectura binaria */
        if (fread(&key, sizeof(key), 1, index_fp) != 1) break; /* fin del fichero */
        if (fread(&offset, sizeof(offset), 1, index_fp) != 1) goto cleanup; /* lectura parcial - error */
        if (fread(&size, sizeof(size), 1, index_fp) != 1) goto cleanup;

        /* inicializacion de book struct */
        book = indexbook_init(key, offset, size);
        if (!book) goto cleanup;

        /* almacenamiento en index struct */
        index->books[index->used++] = book;
    }

    fclose(index_fp);


    return index;

cleanup: 
    {
        if (index_fp) fclose(index_fp);
        if (book) indexbook_free(book);
        if (index) index_free(index);
    }
    return NULL;
}

void index_print(Index *ind) {
    size_t i;
    if (!ind || !ind->books) return;
    for (i = 0 ; i < ind->used ; i++) {
        fprintf(stdout, "Entry #%ld\n", i);
        fprintf(stdout, "\tkey: #%d\n", ind->books[i]->key);
        fprintf(stdout, "\toffset: #%ld\n", ind->books[i]->offset);
        fprintf(stdout, "\tsize: #%ld\n", ind->books[i]->size);        
    }   
}

int index_del(Index *ind, int key) {
    int m;
    int flag = 0;

    if (!ind || !ind->books || ind->used == 0) return -1;

    m = index_find(ind, key);
    if (m == ERR){
        return ERR;
    }
    else if(m != NO_POS){
        indexbook_free(ind->books[m]);
        if (m < (int) ind->used - 1) {
            memmove(&(ind->books[m]), &(ind->books[m+1]), (ind->used - m - 1) * sizeof(IndexBook *));
            ind->used--;
            ind->books[ind->used] = NULL;
        }
        return OK;
    }
    else{
        return NOT_FOUND;
    }
}

int index_find(Index *ind, int key){
    int a, b, m, flag = 0;

    if (ind == NULL || key < 0){
        return ERR;
    }

    a = 0;
    b = ind->used - 1;

    while (a <= b) 
    {
        m = a + (b - a) / 2; /* minimize overflow */

        if (key == ind->books[m]->key) {
            flag = 1;
            return m;
        } else if (key < ind->books[m]->key) {
            b = m - 1;
        } else {
            a = m + 1;
        } 
    }
    
    return NOT_FOUND;
}
