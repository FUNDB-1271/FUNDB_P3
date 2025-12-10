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
            return (CommandCode)i;
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

    if (*command_code == ADD){
        if (book_set_total_size(book, sizeof(int) + MAX_ISBN + strlen(book_get_title(book)) + strlen(book_get_publishedby(book)) + 1) == ERR){
            book_free(book);
            return NULL;
        }
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
            if (command_del(datafile, index, deletedlist, strategy, book_get_id(book)) == ERR){
                return ERR;
            }
            break;
        case FIND:
            command_find(datafile, index, book_get_id(book));
            break;
        case PRINT_IND:
            command_print_ind(index);
            break;
        case PRINT_LST:
            command_print_lst(deletedlist);
            break;
        case PRINT_REC:
            command_print_rec(datafile, index);
            break;
        case EXIT:
            command_exit(datafile, index, filename_root, deletedlist, deletedfile, strategy);
            break;
        default:
            command_unknown();
            break;
    }

    return 0;
}

int command_add(FILE *data_fp, Index *index, DeletedList *deletedlist, Book *book, int strategy) {
    long int index_pos = NO_POS;
    long offset = NO_POS;
    long write_pos;
    int ret = OK;
    IndexBook *indexbook = NULL;
    size_t book_size;

    if (!book || !data_fp || !index || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT) || !deletedlist) return ERR;

    book_size = book_get_total_size(book);
    
    if (!(indexbook = indexbook_init(book_get_id(book), 0, book_size))) return MemError;

    ret = index_find(index, book_get_id(book));
    if (ret != NOT_FOUND) {
        indexbook_free(indexbook);
        return BookExists;
    }

    /* Search for a suitable deleted space */
    index_pos = deletedlist_find(deletedlist, book_size, strategy);
    if (index_pos == ERR) {
        indexbook_free(indexbook);
        return ERR;
    }

    /* If a suitable space was found, use it */
    if (index_pos != NO_POS && index_pos != NOT_FOUND){
        offset = (long)deletedlist_get_offset(deletedlist, index_pos);
    }

    /* Write the book to the database (either at offset or at end) */
    write_pos = database_add(data_fp, offset, book_get_id(book), book_get_title(book), 
                              book_get_isbn(book), book_get_publishedby(book));
    if (write_pos == ERR) {
        indexbook_free(indexbook);
        return WriteError;
    }

    /* Update the indexbook with the actual write position */
    if (indexbook_set_offset(indexbook, write_pos) == ERR){
        indexbook_free(indexbook);
        return ERR;
    }

    /* Update deleted list - this will remove or update the used space */
    ret = deletedlist_update(deletedlist, indexbook, strategy, ADD);
    if (ret == ERR) {
        indexbook_free(indexbook);
        return ERR;
    }

    /* Add to index */
    ret = index_add(index, indexbook);
    if (ret != OK) {
        indexbook_free(indexbook);
        return MemError;
    }

    fflush(data_fp);
    return NoError;
}

int command_del(FILE *data_fp, Index *index, DeletedList *deletedlist, int strategy, int key) {
    int pos;

    if (!data_fp || !index || (strategy != BESTFIT && strategy != WORSTFIT && strategy != FIRSTFIT) || !deletedlist){
        return ERR;
    }

    pos = index_find(index, key);
    if (pos == NOT_FOUND){
        printf("Record with bookId=%d does not exist\n", key);
        return NOT_FOUND;
    }

    if (database_del(data_fp, index, deletedlist, index_get_indexbook(index, pos)) == ERR){
        return ERR;
    }

    if (deletedlist_update(deletedlist, index_get_indexbook(index, pos), strategy, DEL) == ERR){
        return ERR;
    }

    if (index_del(index, key) == ERR){
        return ERR;
    }

    fflush(data_fp);
    printf("Record with BookID=%d has been deleted\n", key);

    return OK;
}

int command_exit(FILE *datafile, Index *index, char *filename_root, DeletedList *deletedlist, FILE *deletedfile, int strategy) {
    char filename[NAME_MAX + 4];

    if (!datafile || !index || !filename_root || !deletedlist || !deletedfile) return ERR;

    snprintf(filename, sizeof(filename), "%s.ind", filename_root);
    index_save(index, filename);
    snprintf(filename, sizeof(filename), "%s.lst", filename_root);
    deletedlist_save(deletedlist, filename, strategy);

    return OK;
}

int command_find(FILE *data_fp, Index *index, int key) {
    if (data_fp == NULL || index == NULL){
        return ERR;
    }

    if (database_find(data_fp, index, key) == ERR){
        return ERR;
    }

    return OK;
}

int command_print_rec(FILE *data_fp, Index *index) {
    if (data_fp == NULL || index == NULL){
        return ERR;
    }

    database_print_rec(data_fp, index);
    return OK;
}

int command_print_lst(DeletedList *deletedlist) {
    if (!deletedlist) return ERR;

    deletedlist_print(deletedlist);

    return OK;
}

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
