#ifndef COMMANDS_H
#define COMMANDS_H

#define N_CMD 8

#include "types.h"
#include "index.h"
#include "delete.h"
#include "database.h"

#include <stdio.h>


typedef enum { NO_CMD = 0, UNKNOWN, ADD, FIND, DEL, EXIT = 5, PRINT_REC, PRINT_IND, PRINT_LST } CommandCode;

typedef struct {
    CommandCode cmdcode;
    int book_id;
    char isbn[MAX_ISBN];
    char title[MAX_TITLE];
    char publishedby[MAX_PUBLISHEDBY];
    size_t total_size;
} Command;

void command_parse(const char *input, Command *cmd);

int command_execute(FILE *datafile, Index *index, FILE *indexfile, /*DeletedList *lst, FILE *deletedfile, */ int strategy, Command command, char *filename_root);

int command_add(FILE *data_fp, Index *index, /*DeletedList *lst,*/ int strategy, Command command);

void command_add_interpret_exit(Command cmd, int exit_code);

int command_del();

int command_exit(FILE *datafile, Index *index, char *filename_root /*, DeletedList *lst, FILE *deletedfile*/);

int command_find();

int command_print_rec();

int command_print_lst();

int command_print_ind();

int command_unknown();

#endif
