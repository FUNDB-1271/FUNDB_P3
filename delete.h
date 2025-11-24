#include "index.h"

#include <stdio.h>

typedef struct _indexdeletedbook IndexDeletedBook;

typedef struct _deletedlist DeletedList;

typedef int (*method_func)(DeletedList*, size_t);

DeletedList *deletedlist_init();

void deletedlist_free(DeletedList *deletedlist);

int deletedlist_add(DeletedList *deletedlist, IndexDeletedBook *indexdeletedbook);

int deletedlist_update(DeletedList *deletedlist, size_t book_size, method_func method);