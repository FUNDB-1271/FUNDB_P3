#ifndef DATABASE_H
#define DATABASE_H

#include "deletedlist.h"
#include "index.h"

int database_add(FILE *database_f, int offset, int book_id, const char *title, const char *isbn, const char *publisher);

int database_find(/* ... */);

int database_del(Index *ind, DeletedList *deletedList, IndexBook *indexbook, int strategy);

int database_exit(/* ... */);

int database_print_rec(/* ... */);

#endif
