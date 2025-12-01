#ifndef COMMANDS_H
#define COMMANDS_H

#define N_CMD 8

#include "types.h"
#include "index.h"
#include "deletedlist.h"
#include "database.h"
#include "book.h"

#include <stdio.h>

typedef enum { NO_CMD = 0, UNKNOWN, ADD, FIND, DEL, EXIT, PRINT_REC, PRINT_IND, PRINT_LST } CommandCode;

Book* command_parse(const char *input, CommandCode *command_code);

int command_execute(FILE *datafile, Index *index, FILE *indexfile, DeletedList *deletedlist, Book *book, FILE *deletedfile, int strategy, CommandCode command_code, char *filename_root);

int command_add(FILE *data_fp, Index *index, DeletedList *deletedlist, Book *book, int strategy);

void command_add_interpret_exit(Book *book, int exit_code);

int command_del(FILE *data_fp, Index *index, DeletedList *deletedlist, int strategy, int key);

int command_exit(FILE *datafile, Index *index, char *filename_root, DeletedList *deletedlist, FILE *deletedfile, int strategy);

int command_find(FILE *data_fp, Index *index, int key);

int command_print_rec();

int command_print_lst();

int command_print_ind(Index *index);

int command_unknown();

#endif
