#include "commands.h"
#include "delete.h"

char *cmd_to_str[N_CMD + 1] = {"No command", "Unknown", "add", "find", "del", "exit", "printRec", "printInd", "printLst"};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief private function for fetching command code
 * 
 * @author Guilherme 
 * @date 26-11-2025
 * @param str user input in cli that is being parsed
 * @return corresponding code or unknown
*/
CommandCode command_code_from_string(const char *str)
{
    if (strcasecmp(str, "add") == 0)   return ADD;
    if (strcasecmp(str, "find") == 0)  return FIND;
    if (strcasecmp(str, "del") == 0)   return DEL;
    if (strcasecmp(str, "exit") == 0)  return EXIT;
    if (strcasecmp(str, "print_rec") == 0) return PRINT_REC;
    if (strcasecmp(str, "print_ind") == 0) return PRINT_IND;
    if (strcasecmp(str, "print_lst") == 0) return PRINT_LST;

    return UNKNOWN;
}

void command_parse(const char *input, Command *cmd)
{
    char buffer[MAX_INPUT];
    strncpy(buffer, input, MAX_INPUT);
    buffer[MAX_INPUT - 1] = '\0';

    cmd->cmdcode = NO_CMD;
    cmd->book_id = 0;
    cmd->isbn[0] = '\0';
    cmd->title[0] = '\0';
    cmd->publishedby[0] = '\0';

    char *token = strtok(buffer, " \t\n");
    if (!token) {
        cmd->cmdcode = UNKNOWN;
        return;
    }

    cmd->cmdcode = command_code_from_string(token);

    if (cmd->cmdcode == EXIT || cmd->cmdcode == UNKNOWN)
        return;

    char *rest = strtok(NULL, "\n");  
    if (!rest) return;

    char *bookid_str   = strtok(rest, "|");
    char *isbn_str     = strtok(NULL, "|");
    char *title_str    = strtok(NULL, "|");
    char *pubby_str    = strtok(NULL, "|");

    if (bookid_str)   cmd->book_id = atoi(bookid_str);

    if (isbn_str)
        strncpy(cmd->isbn, isbn_str, MAX_ISBN - 1),
        cmd->isbn[MAX_ISBN - 1] = '\0';

    if (title_str)
        strncpy(cmd->title, title_str, MAX_TITLE - 1),
        cmd->title[MAX_TITLE - 1] = '\0';

    if (pubby_str)
        strncpy(cmd->publishedby, pubby_str, MAX_PUBLISHEDBY - 1),
        cmd->publishedby[MAX_PUBLISHEDBY - 1] = '\0';
}

int command_execute(FILE *datafile, Index *index, /*DeletedList *lst,*/ int strategy, Command command){

    switch(command.cmdcode)
    {
        case ADD:
            db_add();
    }

}
