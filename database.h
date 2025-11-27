#ifndef DATABASE_H
#define DATABASE_H

#include "delete.h"
#include "index.h"

int database_add(FILE *database_f, int offset, int book_id, char *title, char *isbn, char *publisher);

int database_find(/* ... */);

int database_del(/* ... */);

int database_exit(/* ... */);

int database_print_rec(/* ... */);

#endif
