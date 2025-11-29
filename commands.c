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

Book* command_parse(const char *input, CommandCode *command_code){
    char buffer[MAX_INPUT];
    Book *book = NULL;

    if (input == NULL || command_code == NULL){
        return NULL;
    }

    strncpy(buffer, input, MAX_INPUT);
    buffer[MAX_INPUT - 1] = '\0';

    *command_code = NO_CMD;
    
    char *token = strtok(buffer, " \t\n");
    if (token == NULL) {
        return NULL;
    }

    *command_code = command_code_from_string(token);
    if (*command_code == EXIT || *command_code == UNKNOWN)
        return NULL;

    char *rest = strtok(NULL, "\n");  
    if (!rest) return NULL;

    char *bookid_str   = strtok(rest, "|");
    char *isbn_str     = strtok(NULL, "|");
    char *title_str    = strtok(NULL, "|");
    char *pubby_str    = strtok(NULL, "|");

    if (bookid_str){
        book = book_init(atoi(bookid_str));
        if (book == NULL){
            return NULL;
        }
    }   

    if (isbn_str){
        if (book_set_isbn(book, isbn_str) == ERR){
            book_free(book);
            return NULL;
        }
    }
        
    if (title_str){
        if (book_set_title(book, title_str) == ERR){
            book_free(book);
            return NULL;
        }
    }

    if (pubby_str){
        if (book_set_publishedby(book, pubby_str) == ERR){
            book_free(book);
            return NULL;
        }
    }

    if (book_set_total_size(book, sizeof(int) + MAX_ISBN + strlen(book_get_title(book)) + strlen(book_get_publishedby(book)) + 1) == ERR){
        book_free(book);
        return NULL;
    }

    return book;
}

int command_execute(FILE *datafile, Index *index, FILE *indexfile, DeletedList *deletedlist, Book *book, FILE *deletedfile, int strategy, CommandCode command_code, char *filename_root){
    int aux = 0;

    if (!datafile || !index || !indexfile || !deletedlist || !deletedfile) return ERR;

    switch(command_code)
    {
        case ADD:
            aux = command_add(datafile, index, deletedlist, book, strategy);
            command_add_interpret_exit(book, aux);
            break;
        case DEL:
            command_del(datafile, index, deletedlist, strategy, book_get_id(book));
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
            command_exit(datafile, index, filename_root, deletedlist, deletedfile);
            break;
        default:
            command_unknown();
            break;
    }

    return 0;
}

int command_add(FILE *data_fp, Index *index, DeletedList *deletedlist, Book *book, int strategy) {
    long offset = NO_POS;
    int ret = OK;
    IndexBook *indexbook = NULL;

    if (!(indexbook = indexbook_init(book_get_id(book), 0, book_get_total_size(book)))) return MemError;

    if (!book || !data_fp || !index || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT) || deletedlist) return ERR;

    ret = index_find(index, book_get_id(book));
    if (ret != NOT_FOUND) return BookExists;

    offset = deletedlist_find(deletedlist, book_get_total_size(book), strategy);
    if (offset == ERR || offset == NO_POS) return ERR;

    offset = database_add(data_fp, offset, book_get_id(book), book_get_title(book), book_get_isbn(book), book_get_publishedby(book));
    if (offset == NO_POS) return WriteError;

    indexbook_set_offset(indexbook, offset);

    ret = index_add(index, indexbook);
    if (ret != 0) return MemError;

    ret = deletedlist_update(deletedlist, indexbook, strategy, ADD);
    if (ret == ERR) return ERR;

    fflush(data_fp);
    return NoError;
}

int command_del(FILE *data_fp, Index *index, DeletedList *deletedlist, int strategy, int key) {
    int pos;

    if (!data_fp || !index || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT) || deletedlist){
        return ERR;
    }

    pos = index_find(index, key);
    if (pos == NOT_FOUND){
        return NOT_FOUND;
    }

    if (database_del(index, deletedlist, index_get_indexbook(index, pos), strategy) == ERR){
        return ERR;
    }

    if (index_del(index, key) == ERR){
        return ERR;
    }

    if (deletedlist_update(deletedlist, index_get_indexbook(index, pos), strategy, DEL) == ERR){
        return ERR;
    }

    fflush(data_fp);

    return OK;
}

int command_exit(FILE *datafile, Index *index, char *filename_root, DeletedList *deletedlist, FILE *deletedfile) {
    char filename[NAME_MAX + 4];

    if (!datafile || !index || !filename_root || !deletedlist || !deletedfile) return ERR;

    snprintf(filename, sizeof(filename), "%s.ind", filename_root);
    index_save(index, filename);
    snprintf(filename, sizeof(filename), "%s.del", filename_root);
    deletedlist_save(deletedlist, filename);

    return OK;
}

int command_find() {return OK;}

int command_print_rec() {return OK;}

int command_print_lst() {return OK;}

int command_print_ind(Index *index) {
    if (!index) return ERR;

    index_print(index);

    return OK;
}

int command_unknown() {return OK;}

void command_add_interpret_exit(Book *book, int exit_code) {

        switch(exit_code)
        {
            case NoError:
                fprintf(stdout, "Record with BookID=%d has been added to the database\n", book_get_id(book));
                fflush(stdout);
                break;
            case BookExists: 
                fprintf(stdout, "Record with BookID=%d already exists\n", book_get_id(book));
                fflush(stdout);
                break;
            case WriteError:
                fprintf(stdout, "Error writing to file\n");
                fflush(stdout);
                break;
            case MemError: 
                fprintf(stdout, "Memory failure\n");
                fflush(stdout);
                break;
            default: 
                fprintf(stdout, "Unknown error\n");
                fflush(stdout);
                break;
        }
}