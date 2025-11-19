#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

#define INDEX_INIT_SIZE 4
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

typedef struct _indexbook IndexBook;

typedef struct _index Index;

IndexBook *book_init(int key, long int offset, size_t size);

void book_free(IndexBook *book);

Index *index_init(void);

void index_free(Index *ind);

int index_add(Index *ind, IndexBook *book);

int index_save(Index *ind, char *file);

Index *index_init_from_file(FILE *index_fp);

void index_print(Index *ind);

int index_del(Index *ind, int key);

#endif
