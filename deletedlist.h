#include "index.h"
#include "commands.h"

#define DELETEDLIST_INIT_SIZE 4

#include <stdio.h>

typedef struct _indexdeletedbook IndexDeletedBook;

typedef struct _deletedlist DeletedList;

DeletedList *deletedlist_init();

void deletedlist_free(DeletedList *deletedlist);

size_t deletedlist_get_offset(DeletedList *deletedlist, int pos);

int deletedlist_add(DeletedList *deletedlist, IndexBook *indexbook, int strategy);

int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command_code);

int deletedlist_save(DeletedList *deletedlist, char *file);

int deletedlist_find(DeletedList *deletedlist, size_t book_size, int strategy);

DeletedList *deletedlist_init_from_file(FILE *deletedlist_fp);

void deletedlist_print(DeletedList *deletedList);