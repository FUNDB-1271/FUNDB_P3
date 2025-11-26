#include "commands.h"
#include "delete.h"

#include <stdio.h>

char *cmd_to_str[N_CMD] = {"No command", "Unknown", "add", "find", "del", "exit", "printRec", "printInd", "printLst"};

int db_add(Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){
    int status, pos;

    if (ind == NULL || book == NULL || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT)){
        return -1;
    }
    
    status = index_add(ind, book);
    if (status == -1){
        return -1;
    }
    else if (status == 1){
        printf ("Record with BookID=%ld exists", book_get_key(book));
        return -1;
    }

    pos = deletedlist_find(deletedList, strategy);
    
    /*Falta código para añadir la informacion a la propia bd*/

    if (pos != -1){
        if (deletedlist_update(deletedList, book_get_size(book), strategy) == -1){
            return -1;
        }
    }

    return 0;
}

void db_find(/* ... */){

}

void db_del(Index *ind, DeletedList *deletedList, IndexBook *book, int strategy){

    /*Falta código para eliminar la informacion de la propia bd*/
    
    
    if (index_del(ind, book) == -1){
        return -1;
    }

    if (deletedlist_update(deletedList, book_get_size(book), strategy) == -1){
        return -1;
    }

    return 0;
}

int db_exit(){
    return EXIT;
}

void db_print_ind(/* ... */){

}

void db_print_lst(/* ... */){

}

void db_print_rec(/* ... */){

}



