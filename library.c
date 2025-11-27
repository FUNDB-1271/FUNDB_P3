#include "types.h"
#include "commands.h"
#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXITCODE -2

#define BEST_FIT "best_fit"
#define WORST_FIT "worst_fit"
#define FIRST_FIT "first_fit"

typedef struct {
    FILE *data_fp;
    FILE *index_fp;
    FILE *deleted_fp;
/*   
    DeletedList *del_list;
*/
    Index *index;
    int strategy;
    char dbname[NAME_MAX];
} DBInfo;


Bool dbinfo_error(DBInfo *db);

int _library_init(char *dbname, char *strategy, DBInfo *database);

void _library_cleanup(DBInfo **database);

void loop();

int main(int argc, char *argv[]) {
    DBInfo *database = NULL;

    /* no tocar este mensaje */
    if (argc != 3) {
        fprintf(stdout, "Missing argument");
        return 0;
    }

    /* no tocar este mensaje */
    if (strcmp(argv[1], BEST_FIT) != 0 && strcmp(argv[1], WORST_FIT) != 0 && strcmp(argv[1], FIRST_FIT) != 0) {
        fprintf(stdout, "Unknown search strategy %s", argv[1]);
        return 0;
    }

    if (strlen(argv[2]) > NAME_MAX - 1) {
        fprintf(stderr, "File name [%s] too large, maximum allowed length of path: %d\n", argv[2], NAME_MAX - 1);
        return ERR;
    }

    if (!(database = (DBInfo *) malloc(sizeof(DBInfo)))) {
        fprintf(stderr, "Error reservando memoria para base de datos\n");
        return ERR;
    }

    if (_library_init(argv[2], argv[1], database) != OK) {
        fprintf(stderr, "Failed to init database\n");
        _library_cleanup(&database);
        return ERR;
    }

    loop(database);

    _library_cleanup(&database);


    return 0;
}

int _library_init(char *dbname, char *strategy, DBInfo *database){
    char dataf_name[NAME_MAX];
    char indexf_name[NAME_MAX];
    char deletedf_name[NAME_MAX];
    size_t len;
    
    if (!dbname || !strategy || !database) return ERR;

    snprintf(dataf_name, sizeof(dataf_name), "%s.db", dbname);
    snprintf(indexf_name, sizeof(indexf_name), "%s.ind", dbname);
    snprintf(deletedf_name, sizeof(deletedf_name), "%s.lst", dbname);

    if (strcmp(strategy, BEST_FIT) == 0) {
        database->strategy = BESTFIT;
    } else if (strcmp(strategy, WORST_FIT) == 0) { 
        database->strategy = WORSTFIT;
    } else {
        database->strategy = FIRSTFIT;
    }

    len = (strlen(dbname) < NAME_MAX - 1) ? strlen(dbname) : NAME_MAX - 1;
    strncpy(database->dbname, dbname, len);
    database->dbname[len] = '\0';

    database->data_fp = fopen(dataf_name, "r+b");
    if (!(database->data_fp)) database->data_fp = fopen(dataf_name, "w+b");
    if (!(database->data_fp)) {
        fprintf(stderr, "Error opening or creating file: %s\n", dataf_name);
        return ERR;
    }
    
    database->index_fp = fopen(indexf_name, "r+b");
    if (!(database->index_fp)) database->index_fp = fopen(indexf_name, "w+b");
    if (!(database->index_fp)) {
        fprintf(stderr, "Error opening or creating file: %s\n", indexf_name);
        fclose(database->data_fp);
        return ERR;
    }

    database->deleted_fp = fopen(deletedf_name, "r+b");
    if (!(database->deleted_fp)) database->deleted_fp = fopen(deletedf_name, "w+b");
    if (!(database->deleted_fp)) {
        fprintf(stderr, "Error opening or crating file: %s\n", deletedf_name);
        fclose(database->data_fp);
        fclose(database->index_fp);
        return ERR;
    }

    if (!(database->index = index_init_from_file(database->index_fp))) {
        fprintf(stderr, "Error loading index structure\n");
        fclose(database->data_fp);        
        fclose(database->index_fp);
        fclose(database->deleted_fp);
        return ERR;        
    }


/*
    if (!(database->del_list = deletedlist_init_from_file(database->dbname))) {
        fprintf(stderr, "Error loading deleted list structure\n");
        index_free(database->index);
        fclose(database->data_fp);
        fclose(database->index_fp);
        fclose(database->deleted_fp);
        return ERR;
    }
*/
    return OK;
}

void loop(DBInfo *database) 
{
    char user_input[MAX_INPUT];
    Command current_command = {0};

    if (dbinfo_error(database)) return; 

    fprintf(stdout, "Type command and argument/s.\n");
    fprintf(stdout, "exit\n");
    fflush(stdout);

    while (1) {
        /* read from stdin accepting either '\n' or '\r' as EOL */
        int i = 0;
        int c = 0;

        /* initialize buffer */
        user_input[0] = '\0';

        /* read char-by-char */
        while ((c = fgetc(stdin)) != EOF) {
            if (c == '\r') {
                /* consume optional following LF so CRLF becomes a single terminator */
                int next = fgetc(stdin);
                if (next != '\n' && next != EOF) {
                    ungetc(next, stdin);
                }
                break; /* treat as end-of-line */
            }
            if (c == '\n') {
                break; /* end-of-line */
            }
            if (i < MAX_INPUT - 1) {
                user_input[i++] = (char)c;
            }
        }

        if (c == EOF && i == 0) {
            /* EOF and nothing read -> exit loop */
            break;
        }

        /* if we reached EOF but collected data, terminate */
        if (i >= MAX_INPUT - 1) user_input[MAX_INPUT - 1] = '\0';

        /* If nothing but an empty line, continue waiting for input */
        if (strlen(user_input) == 0) continue;

        command_parse(user_input, &current_command);

        command_execute(database->data_fp, database->index, database->index_fp,
                        database->strategy, current_command, database->dbname);

        /* PRINT EXIT ONLY IF NOT LAST COMMAND */
        if (current_command.cmdcode != EXIT) {
            fprintf(stdout, "exit\n");
            fflush(stdout);
        } else {
            fflush(stdout);
            break;   
        }

    }
}

void _library_cleanup(DBInfo **database) {
    if(*database) {
        if ((*database)->data_fp) 
        { 
            fclose((*database)->data_fp); 
            (*database)->data_fp = NULL; 
        }
        if ((*database)->index_fp) 
        { 
            fclose((*database)->index_fp); 
            (*database)->index_fp = NULL; 
        }
        if ((*database)->deleted_fp) 
        { 
            fclose((*database)->deleted_fp); 
            (*database)->deleted_fp = NULL; 
        }
/*
        if ((*database)->del_list) 
        { 
            deletedlist_free((*database)->del_list); 
            (*database)->del_list = NULL; 
        }
*/  
        if ((*database)->index) 
        { 
            index_free((*database)->index); 
            (*database)->index = NULL; 
        }
        free(*database);
        *database = NULL;
    }
}

Bool dbinfo_error(DBInfo *db)
{
    if (!db) return true;
    if (!db->data_fp) return true;
    if (!db->deleted_fp) return true;
    if (!db->index_fp) return true;
    if (!db->index) return true;
/*
    if (!db->del_list) return true;
*/
    if (db->strategy != BESTFIT && db->strategy != WORSTFIT && db->strategy != FIRSTFIT) return true;

    return false;
}
