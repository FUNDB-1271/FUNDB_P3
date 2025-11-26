#include "database.h"

#include <stdio.h>
#include <stdlib.h>

int database_add(FILE *fdb, Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){
    int status, pos;
    size_t offset;

    if (ind == NULL || book == NULL || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT)){
        return ERR;
    }
    
    pos = deletedlist_find(deletedList,indexbook_get_size(book), strategy);
    if (pos == ERR){
        return ERR;
    }
    else if (pos == NO_POS){
        fseek(fdb, 0, SEEK_END);
        offset = ftell(fdb);
    }
    else{
        offset = deletedlist_get_offset(deletedList, pos);
        if (offset == 0){
            return ERR;
        }
    }

    if (indexbook_set_offset(book, offset) == ERR){
        return ERR;
    }

    fseek(fdb, offset, SEEK_SET);
    if (indexbook_write(fdb, book) == ERR){
        return ERR;
    }
    
    status = index_add(ind, book);
    if (status == ERR){
        return ERR;
    }
    else if (status == 1){
        printf ("Record with BookID=%ld exists", indexbook_get_key(book));
        return ERR;
    }

    /*Falta meter la funcion que agrega un libro a la base de datos */

    if (pos != ERR){
        if (deletedlist_update(deletedList, indexbook_get_size(book), strategy) == ERR){
            return ERR;
        }
    }

    printf("Record with BookID=%ld has been added to the database\n",
            indexbook_get_key(book));

    return 0;
}

int database_find(FILE *fdb, Index *ind, int key){
    int pos;
    IndexBook *book = NULL;

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

    if (fseek(fdb, , SEEK_SET) != 0){
        return ERR;
    }

    if (indexbook_read(fdb, indexbook_get_offset(),) == ERR){
        return ERR;
    }

    /*Hacer el print*/
    
    return OK;
}

void database_del(Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){

    /*Falta código para eliminar la informacion de la propia bd*/
    
    
    if (index_del(ind, book) == ERR){
        return ERR;
    }

    if (deletedlist_update(deletedList, indexbook_get_size(book), strategy) == ERR){
        return ERR;
    }

    return 0;
}

int database_exit(){
    return EXIT;
}

void database_print_ind(){
    index_print()

}

void database_print_lst(/* ... */){

}

void database_print_rec(/* ... */){

}



