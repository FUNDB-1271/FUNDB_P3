#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long database_add(FILE *database_f, long offset, int book_id, const char *title, const char *isbn, const char *publisher) {
    size_t title_length, publisher_length, total_length;
    char isbn_cpy[MAX_ISBN], a = '|';
    long chosen_pos, file_size, write_pos;
    size_t copy_length = 0;

    if (!database_f || !title || !isbn || !publisher) return ERR;

    /* find file size */
    if (fseek(database_f, 0, SEEK_END) != 0) return ERR;
    file_size = ftell(database_f);

    /* decide write position */
    if (offset == NO_POS) {
        chosen_pos = file_size; /* append at end */
    } else {
        /* use the provided offset for reusing deleted space */
        chosen_pos = offset;
    }

    /* Save the actual write position */
    write_pos = chosen_pos;

    /** compute length of fields*/
    title_length = strlen(title) < MAX_TITLE ? strlen(title) : MAX_TITLE;
    publisher_length = strlen(publisher) < MAX_PUBLISHEDBY ? strlen(publisher) : MAX_PUBLISHEDBY;

    total_length = sizeof(int) + MAX_ISBN + title_length + 1 + publisher_length;

    /** truncate isbn or pad it with zeros in case it is shorter than MAX_ISBN */
    copy_length = strlen(isbn);    
    if (copy_length > MAX_ISBN) copy_length = MAX_ISBN;

    /* fill with zeros to pad to MAX_ISBN */
    memset(isbn_cpy, '\0', MAX_ISBN);
    memcpy(isbn_cpy, isbn, copy_length);

    /* Position for writing */
    if (fseek(database_f, chosen_pos, SEEK_SET) != 0) return ERR;

    /* Write new record */
    if (fwrite(&total_length, sizeof(size_t), 1, database_f) != 1) return ERR;
    if (fwrite(&book_id, sizeof(int), 1, database_f) != 1) return ERR;
    if (fwrite(isbn_cpy, sizeof(char), MAX_ISBN, database_f) != MAX_ISBN) return ERR;
    if (fwrite(title, sizeof(char), title_length, database_f) != title_length) return ERR;
    if (fwrite(&a, sizeof(char), 1, database_f) != 1) return ERR;
    if (fwrite(publisher, sizeof(char), publisher_length, database_f) != publisher_length) return ERR;

    fflush(database_f);
    return write_pos;  /* Return actual write position */
}

int database_del(FILE *database_f, Index *ind, DeletedList *deletedList, IndexBook *indexbook){
    long offset;
    size_t size;
    
    if (!ind || !deletedList || !indexbook){
        return ERR;
    } 

    /* Get offset and size of the record */
    offset = indexbook_get_offset(indexbook);
    size   = indexbook_get_size(indexbook);
    
    if (offset < 0 || size == 0)
        return ERR;

    /* Position at the beginning of the record */
    if (fseek(database_f, offset, SEEK_SET) != 0) {
        return ERR;
    }

    fflush(database_f);

    return OK;
}

int database_find(FILE *fdb, Index *ind, int key){
    int pos;
    size_t total_length;
    int book_id;
    char isbn[MAX_ISBN + 1];
    char *title_ptr, *publisher_ptr;
    char *buffer;
    char *separator_ptr;

    if (fdb == NULL || ind == NULL || key < 0){
        return ERR;
    }

    pos = index_find(ind, key);
    if (pos == ERR){
        return ERR;
    }
    else if (pos == NOT_FOUND){
        printf("Record with bookId=%d does not exist\n", key);
        return NOT_FOUND;
    }

    if (fseek(fdb, indexbook_get_offset(index_get_indexbook(ind, pos)), SEEK_SET) != 0){
        return ERR;
    }

    /* Read size_t total_length */
    if (fread(&total_length, sizeof(size_t), 1, fdb) != 1)
        return ERR;
    
    /* Read book_id */
    if (fread(&book_id, sizeof(int), 1, fdb) != 1)
        return ERR;
    
    /* Read ISBN */
    if (fread(isbn, sizeof(char), MAX_ISBN, fdb) != MAX_ISBN)
        return ERR;
    isbn[MAX_ISBN] = '\0';
    
    /* Rest of record is title + '|' + publisher */
    size_t remaining_size = total_length - sizeof(int) - MAX_ISBN;
    buffer = malloc(remaining_size + 1);
    if (buffer == NULL){
        return ERR;
    }
    
    if (fread(buffer, sizeof(char), remaining_size, fdb) != remaining_size){
        free(buffer);
        return ERR;
    }
    buffer[remaining_size] = '\0';
    
    /* Find separator '|' */
    separator_ptr = memchr(buffer, '|', remaining_size);
    if (separator_ptr == NULL) {
        free(buffer);
        return ERR;
    }
    
    /* Separate title and publisher */
    *separator_ptr = '\0';
    title_ptr = buffer;
    publisher_ptr = separator_ptr + 1;
    
    /* Print in format: book_id|isbn|title|publisher */
    printf("%d|%s|%s|%s\n", book_id, isbn, title_ptr, publisher_ptr);
    
    free(buffer);
    return OK;
}

int database_exit(){
    return 0;
}

int database_print_rec(FILE *database_f, Index *index){
    size_t i;
    size_t total_length;
    int book_id;
    char isbn[MAX_ISBN + 1];
    char *buffer;
    char *separator_ptr;
    IndexBook *indexbook;

    if (database_f == NULL || index == NULL){
        return ERR;
    }

    for (i = 0; i < index_get_used(index); i++){
        /* Get book from index at position i */
        indexbook = index_get_indexbook(index, i);
        if (indexbook == NULL) {
            continue;
        }

        /* Go to record position in file */
        if (fseek(database_f, indexbook_get_offset(indexbook), SEEK_SET) != 0) {
            return ERR;
        }

        /* Read size_t total_length */
        if (fread(&total_length, sizeof(size_t), 1, database_f) != 1)
            return ERR;
        
        /* Read book_id */
        if (fread(&book_id, sizeof(int), 1, database_f) != 1)
            return ERR;
        
        /* Read ISBN */
        if (fread(isbn, sizeof(char), MAX_ISBN, database_f) != MAX_ISBN)
            return ERR;
        isbn[MAX_ISBN] = '\0';
        
        /* Rest of record is title + '|' + publisher */
        size_t remaining_size = total_length - sizeof(int) - MAX_ISBN;
        buffer = malloc(remaining_size + 1);
        if (buffer == NULL){
            return ERR;
        }
        
        if (fread(buffer, sizeof(char), remaining_size, database_f) != remaining_size){
            free(buffer);
            return ERR;
        }
        buffer[remaining_size] = '\0';
        
        /* Find separator '|' */
        separator_ptr = memchr(buffer, '|', remaining_size);
        if (separator_ptr == NULL) {
            free(buffer);
            return ERR;
        }
        
        /* Separate title and publisher */
        *separator_ptr = '\0';
        char *title_ptr = buffer;
        char *publisher_ptr = separator_ptr + 1;
        
        /* Print in format: book_id|isbn|title|publisher */
        printf("%d|%s|%s|%s\n", book_id, isbn, title_ptr, publisher_ptr);
        
        free(buffer);
    }

    return OK;
}

int database_print_size(FILE *fdb, size_t size, long offset)
{
    long aux = 0;

    if (!fdb || offset < 0) return ERR;

    if ((aux = (long) fseek(fdb, 0, SEEK_END)) != 0) return ERR;

    if (offset > aux) return ERR;

    if ((aux = (long) fseek(fdb, 0, offset) != 0)) return ERR;

    if (fwrite(&size, sizeof(size_t), 1, fdb) != 1) return ERR;

    return OK;
}