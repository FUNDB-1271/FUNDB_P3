

#include <stdio.h>

typedef struct _indexdeletedbook IndexDeletedBook;

typedef struct _deletedlist DeletedList;


DeletedList *deletedlist_init();

void deletedlist_free(DeletedList *deletedlist);

int deletedlist_add(DeletedList *deletedlist, IndexDeletedBook *indexdeletedbook);

int deletedlist_update(DeletedList *deletedlist);