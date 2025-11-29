#ifndef DATABASE_H
#define DATABASE_H

#include "deletedlist.h"
#include "index.h"

int database_add(FILE *database_f, size_t offset, int book_id, const char *title, const char *isbn, const char *publisher);

int database_find(FILE *fdb, Index *ind, int key);

int database_del(FILE *database_f, Index *ind, DeletedList *deletedList, IndexBook *indexbook);

int database_exit(/* ... */);

int database_print_rec(/* ... */);

#endif
