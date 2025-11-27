#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *cmd_to_str[N_CMD + 1] = {"No command", "Unknown", "add", "find", "del", "exit", "printRec", "printInd", "printLst"};
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
    for (int i = 0; i <= N_CMD; i++) {
        if (cmd_to_str[i] == NULL)
            continue;

        if (strcasecmp(str, cmd_to_str[i]) == 0) {
            return (CommandCode)i;   // index matches enum value
        }
    }

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

    cmd->total_size = sizeof(int) + MAX_ISBN + strlen(cmd->title) + strlen(cmd->publishedby) + 1; /* total size of buffer written to memory */
}

int command_execute(FILE *datafile, Index *index, FILE *indexfile, /*DeletedList *lst, FILE *deletedfile, */ int strategy, Command command, char *filename_root){
    int aux = 0;

    if (!datafile || !index || !indexfile /* || !lst || !deletedfile*/) return ERR;

    switch(command.cmdcode)
    {
        case ADD:
            aux = command_add(datafile, index, /* lst, */strategy, command);
            command_add_interpret_exit(command, aux);
            break;
        case DEL:
            command_del();
            break;
        case FIND:
            command_find();
            break;
        case PRINT_IND:
            command_print_ind(index);
            break;
        case PRINT_LST:
            command_print_lst();
            break;
        case PRINT_REC:
            command_print_rec();
            break;
        case EXIT:
            command_exit(datafile, index, filename_root /*, lst, deletedfile*/);
            break;
        default:
            command_unknown();
            break;
    }

    return 0;
}

int command_add(FILE *data_fp, Index *index, /*DeletedList *lst,*/ int strategy, Command command) {
    long offset = NO_POS;
    int ret = OK;
    IndexBook *indexbook = NULL;

    if (!(indexbook = indexbook_init(command.book_id, 0, command.total_size))) return MemError;

    if (!data_fp || !index || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT) /* || !lst*/) return ERR;

    ret = index_find(index, command.book_id);
    if (ret != NOT_FOUND) return BookExists;
/*
    offset = deletedlist_find(lst, command.total_size, strategy);
*/
    offset = database_add(data_fp, offset, command.book_id, command.title, command.isbn, command.publishedby);
    if (ret == ERR) return WriteError;

    indexbook_set_offset(indexbook, offset);

    ret = index_add(index, indexbook);
    if (ret != 0) return MemError;

/*
    ret = deletedlist_update_element(lst, offset, shift);
*/

    fflush(data_fp);
    return NoError;
}

int command_del() {}

int command_exit(FILE *datafile, Index *index, char *filename_root /*, DeletedList *lst */) {
    char filename[NAME_MAX + 4];

    if (!datafile || !index || !filename_root /*|| !lst || !deletedfile*/) return ERR;

    snprintf(filename, sizeof(filename), "%s.ind", filename_root);
    index_save(index, filename);
/*
    deleted_save(lst, deletedfile);
*/  
    return OK;
}

int command_find() {}

int command_print_rec() {}

int command_print_lst() {}

int command_print_ind(Index *index) {
    if (!index) return ERR;

    index_print(index);

    return OK;
}

int command_unknown() {}

void command_add_interpret_exit(Command cmd, int exit_code) {

        switch(exit_code)
        {
            case NoError:
                fprintf(stdout, "Record with BookID=%d has been added to the database", cmd.book_id);
                fflush(stdout);
                break;
            case BookExists: 
                fprintf(stdout, "Record with BookID=%d already exists", cmd.book_id);
                fflush(stdout);
                break;
            case WriteError:
                fprintf(stdout, "Error writing to file");
                fflush(stdout);
                break;
            case MemError: 
                fprintf(stdout, "Memory failure");
                fflush(stdout);
                break;
            default: 
                fprintf(stdout, "Unknown error");
                fflush(stdout);
                break;
        }
}