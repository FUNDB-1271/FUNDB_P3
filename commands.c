#include "commands.h"
#include "delete.h"

#include <stdio.h>

char *cmd_to_str[N_CMD] = {"No command", "Unknown", "add", "find", "del", "exit", "printRec", "printInd", "printLst"};

int db_add(FILE *fdb, Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){
    int status, pos;
    size_t offset;

    if (ind == NULL || book == NULL || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT)){
        return ERR;
    }
    
    pos = deletedlist_find(deletedList,book_get_size(book), strategy);
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

    if (book_set_offset(book, offset) == ERR){
        return ERR;
    }

    fseek(fdb, offset, SEEK_SET);
    if (book_write(fdb, book) == ERR){
        return ERR;
    }
    
    status = index_add(ind, book);
    if (status == ERR){
        return ERR;
    }
    else if (status == 1){
        printf ("Record with BookID=%ld exists", book_get_key(book));
        return ERR;
    }

    /*Falta meter la funcion que agrega un libro a la base de datos */

    if (pos != ERR){
        if (deletedlist_update(deletedList, book_get_size(book), strategy) == ERR){
            return ERR;
        }
    }

    printf("Record with BookID=%ld has been added to the database\n",
            book_get_key(book));

    return 0;
}

void db_find(/* ... */){

}

void db_del(Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){

    /*Falta código para eliminar la informacion de la propia bd*/
    
    
    if (index_del(ind, book) == ERR){
        return ERR;
    }

    if (deletedlist_update(deletedList, book_get_size(book), strategy) == ERR){
        return ERR;
    }

    return 0;
}

int db_exit(){
    return EXIT;
}

void db_print_ind(){
    index_print()

}

void db_print_lst(/* ... */){

}

void db_print_rec(/* ... */){

}



