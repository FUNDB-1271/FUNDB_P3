#include "index.h"

#include <stdio.h>

typedef struct _indexdeletedbook IndexDeletedBook;

typedef struct _deletedlist DeletedList;

DeletedList *deletedlist_init();

void deletedlist_free(DeletedList *deletedlist);

size_t deletedlist_get_offset(DeletedList *deletedlist, int pos);

int deletedlist_add(DeletedList *deletedlist, IndexDeletedBook *indexdeletedbook);

int deletedlist_update(DeletedList *deletedlist, size_t book_size, int strategy);

int deletedlist_find(DeletedList *deletedlist, size_t book_size, int strategy);

void deletedlist_print(DeletedList *deletedList);