#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

#include "types.h"

#define INDEX_INIT_SIZE 4
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

typedef struct _indexbook IndexBook;

typedef struct _index Index;

IndexBook *book_init(int key, long int offset, size_t size);

void book_free(IndexBook *book);

size_t book_get_size(IndexBook *book);

int book_get_key(IndexBook *book);

int book_set_offset(IndexBook *book, size_t offset);

long int book_get_offset(IndexBook *book);

Index *index_init(void);

void index_free(Index *ind);

int index_add(Index *ind, IndexBook *book);

int index_save(Index *ind, char *file);

Index *index_init_from_file(FILE *index_fp);

void index_print(Index *ind);

int index_del(Index *ind, int key);

int index_find(Index *ind, int key);

#endif
