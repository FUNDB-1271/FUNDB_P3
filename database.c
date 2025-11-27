#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int database_add(FILE *database_f, int offset, int book_id, char *title, char *isbn, char *publisher) {
    size_t title_length, publisher_length, total_length;
    char isbn_cpy[MAX_ISBN], a = '|';
    long chosen_pos, pos, file_size;
    int need_restore = 0;
    
    size_t aux_length = 0;
    char aux[MAX_INPUT];

    if (!database_f || !title || !isbn || !publisher) return ERR;

    /* find file size */
    if (fseek(database_f, 0, SEEK_END) != 0) return ERR;
    file_size = ftell(database_f);
    if (file_size < 0) return ERR;

    /* decide write position */
    if (offset < 0) {
        chosen_pos = file_size; /* append */
    } else {
        /* clamp offset to file end if it's beyond EOF */
        chosen_pos = (offset > file_size) ? file_size : offset;
    }

    if (chosen_pos < file_size) {
        if (fseek(database_f, chosen_pos, SEEK_SET) != 0) return ERR;

        /* read existing record length */
        if (fread(&aux_length, sizeof(size_t), 1, database_f) != 1) return ERR;

        if (fread(aux, sizeof(char), aux_length, database_f) != aux_length) return ERR;
        need_restore = 1;
    } else {
        /* no existing data to back up (appending at EOF or file empty) */
        aux_length = 0;
    }
    /** compute length of fields*/
    title_length = strlen(title) < MAX_TITLE ? strlen(title) : MAX_TITLE;
    publisher_length = strlen(publisher) < MAX_PUBLISHEDBY ? strlen(publisher) : MAX_PUBLISHEDBY;

    total_length = sizeof(int) + MAX_ISBN + title_length + 1 + publisher_length; 

    /** truncate isbn or pad it with zeros in case it is shorter than MAX_ISBN */
    memset(isbn_cpy, 0, MAX_ISBN);
    strncpy(isbn_cpy, isbn, MAX_ISBN - 1);
    isbn_cpy[MAX_ISBN - 1] = '\0';

    fseek(database_f, chosen_pos, SEEK_SET);

    pos = ftell(database_f);

    if (fwrite(&total_length, sizeof(size_t), 1, database_f) != 1) goto restore;
    if (fwrite(&book_id, sizeof(int), 1, database_f) != 1) goto restore;
    if (fwrite(isbn_cpy, sizeof(char), MAX_ISBN, database_f) != MAX_ISBN) goto restore;
    if (fwrite(title, sizeof(char), title_length, database_f) != title_length) goto restore;
    if (fwrite(&a, sizeof(char), 1, database_f) != 1) goto restore;
    if (fwrite(publisher, sizeof(char), publisher_length, database_f) != publisher_length) goto restore;

    fflush(database_f);

    return pos;

restore:
{
    if (need_restore) 
    {
        fseek(database_f, pos, SEEK_SET);
        fwrite(&aux_length, sizeof(size_t), 1, database_f);
        fwrite(aux, sizeof(char), aux_length, database_f);
        fflush(database_f);
    }
}
    return ERR;
}

int database_find(FILE *fdb, Index *ind, int key){
    // int pos;
    // IndexBook *book = NULL;

    // if (fdb == NULL || ind == NULL || key < 0){
    //     return ERR;
    // }

    // pos = index_find(ind, key);
    // if (pos == ERR){
    //     return ERR;
    // }
    // else if (pos == NOT_FOUND){
    //     printf("Record with bookId=%d does not exist\n", key);
    //     return NOT_FOUND;
    // }

    // if (fseek(fdb, , SEEK_SET) != 0){
    //     return ERR;
    // }

    // if (indexbook_read(fdb, indexbook_get_offset(),) == ERR){
    //     return ERR;
    // }

    /*Hacer el print*/
    
    return OK;
}

int database_del(Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){

    /*Falta código para eliminar la informacion de la propia bd*/
    
    
    // if (index_del(ind, book) == ERR){
    //     return ERR;
    // }

    // if (deletedlist_update(deletedList, indexbook_get_size(book), strategy) == ERR){
    //     return ERR;
    // }

    // return 0;
}

int database_exit(){
    return 0;
}

int database_print_rec(FILE *database_f){

}



