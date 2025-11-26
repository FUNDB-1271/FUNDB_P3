#ifndef COMMANDS_H
#define COMMANDS_H

#define N_CMD 8

#include "types.h"
#include "index.h"

#include <stdio.h>


typedef enum { NO_CMD = -1, UNKNOWN, ADD, FIND, DEL, EXIT, PRINT_REC, PRINT_IND, PRINT_LST } CommandCode;

typedef struct {
    CommandCode cmdcode;
    int book_id;
    char isbn[MAX_ISBN];
    char title[MAX_TITLE];
    char publishedby[MAX_PUBLISHEDBY];
} Command;

void command_parse(const char *input, Command *cmd);

int command_execute(FILE *datafile, Index *index, /*DeletedList *lst,*/ int strategy, Command command);

#endif