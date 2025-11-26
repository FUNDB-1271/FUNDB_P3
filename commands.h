#ifndef COMMANDS_H
#define COMMANDS_H

#define N_CMD 8

#include "types.h"

typedef enum { NO_CMD = -1, UNKNOWN, ADD, FIND, DEL, EXIT, PRINT_REC, PRINT_IND, PRINT_LST } CommandCode;

int db_add(FILE *fdb, Index *ind, DeletedList *deletedList, IndexBook *book, int strategy);
void db_find(/* ... */);

void db_del(/* ... */);

void db_exit(/* ... */);

void db_print_ind(/* ... */);

void db_print_lst(/* ... */);

void db_print_rec(/* ... */);

#endif