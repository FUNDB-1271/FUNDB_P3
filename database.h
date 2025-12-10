#ifndef DATABASE_H
#define DATABASE_H

#include "deletedlist.h"
#include "index.h"

long database_add(FILE *database_f, long offset, int book_id, const char *title, const char *isbn, const char *publisher);

int database_find(FILE *fdb, Index *ind, int key);

int database_del(FILE *database_f, Index *ind, DeletedList *deletedList, IndexBook *indexbook);

int database_print_size(FILE *fdb, size_t size, long offset);

int database_exit(/* ... */);

int database_print_rec(/* ... */);

#endif
