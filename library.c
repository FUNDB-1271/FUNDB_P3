/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Ejecutable principal de la base de datos.
 *      Implementa inicialización de ficheros (.db, .ind, .lst), carga de 
 *      estructuras Index y DeletedList, bucle principal de comandos y
 *      limpieza de recursos.
 ******************************************************************************/

#include "types.h"
#include "commands.h"
#include "index.h"
#include "deletedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXITCODE -2
#define BEST_FIT "best_fit"
#define WORST_FIT "worst_fit"
#define FIRST_FIT "first_fit"

/* Estructura que mantiene agrupada toda la información de la BD */
typedef struct {
    FILE *data_fp;
    FILE *index_fp;
    FILE *deleted_fp; 
    DeletedList *deletedlist;
    Index *index;
    int strategy;
    char dbname[NAME_MAX];
} DBInfo;

/* Prototipos internos */
Bool dbinfo_error(DBInfo *db);
int _library_init(char *dbname, char *strategy, DBInfo *database);
void _library_cleanup(DBInfo **database);
void loop(DBInfo *database);

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Función principal que inicializa la base de datos, ejecuta el bucle de
 *      comandos y realiza la limpieza final.
 * Parámetros:
 *      argc - número de argumentos
 *      argv - array de argumentos [estrategia, nombre_bd]
 * Retorno:
 *      Código de salida del programa
 ******************************************************************************/
int main(int argc, char *argv[]) {
    DBInfo *database = NULL;

    /* no tocar este mensaje */
    if (argc != 3) {
        fprintf(stdout, "Missing argument");
        return 0;
    }

    /* no tocar este mensaje */
    if (strcmp(argv[1], BEST_FIT) != 0 &&
        strcmp(argv[1], WORST_FIT) != 0 &&
        strcmp(argv[1], FIRST_FIT) != 0) {
        fprintf(stdout, "Unknown search strategy %s", argv[1]);
        return 0;
    }

    /* Comprobación del tamaño del nombre */
    if (strlen(argv[2]) > NAME_MAX - 1) {
        fprintf(stderr, "File name [%s] too large, maximum allowed length of path: %d\n",
                argv[2], NAME_MAX - 1);
        return ERR;
    }

    /* Reserva de estructura DBInfo */
    if (!(database = (DBInfo *) malloc(sizeof(DBInfo)))) {
        fprintf(stderr, "Error reservando memoria para base de datos\n");
        return ERR;
    }

    /* Inicialización completa (ficheros + estructuras) */
    if (_library_init(argv[2], argv[1], database) != OK) {
        fprintf(stderr, "Failed to init database\n");
        _library_cleanup(&database);
        return ERR;
    }

    /* Bucle principal de comandos */
    loop(database);

    /* Liberación completa */
    _library_cleanup(&database);

    return 0;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Inicializa la base de datos:
 *          - Construye los nombres de ficheros (.db, .ind, .lst)
 *          - Selecciona la estrategia de búsqueda de huecos
 *          - Abre o crea los ficheros correspondientes
 *          - Carga de estructuras Index y DeletedList desde fichero
 * Parámetros:
 *      dbname   - nombre base de la base de datos
 *      strategy - estrategia de búsqueda (best_fit, worst_fit, first_fit)
 *      database - puntero a la estructura DBInfo donde guardar toda la info
 * Retorno:
 *      OK si se inicializa correctamente, ERR en caso de fallo
 ******************************************************************************/
int _library_init(char *dbname, char *strategy, DBInfo *database){
    char dataf_name[NAME_MAX];
    char indexf_name[NAME_MAX];
    char deletedf_name[NAME_MAX];
    size_t len;
    
    if (!dbname || !strategy || !database) return ERR;

    /* Construcción de nombres de fichero */
    snprintf(dataf_name, sizeof(dataf_name), "%s.db", dbname);
    snprintf(indexf_name, sizeof(indexf_name), "%s.ind", dbname);
    snprintf(deletedf_name, sizeof(deletedf_name), "%s.lst", dbname);

    /* Asignación de estrategia */
    if (strcmp(strategy, BEST_FIT) == 0) {
        database->strategy = BESTFIT;
    } else if (strcmp(strategy, WORST_FIT) == 0) { 
        database->strategy = WORSTFIT;
    } else {
        database->strategy = FIRSTFIT;
    }

    /* Copia del nombre base de la base de datos */
    len = (strlen(dbname) < NAME_MAX - 1) ? strlen(dbname) : NAME_MAX - 1;
    strncpy(database->dbname, dbname, len);
    database->dbname[len] = '\0';

    /* Apertura del fichero .db (datos) */
    database->data_fp = fopen(dataf_name, "r+b");
    if (!(database->data_fp)) database->data_fp = fopen(dataf_name, "w+b");
    if (!(database->data_fp)) {
        fprintf(stderr, "Error opening or creating file: %s\n", dataf_name);
        return ERR;
    }
    
    /* Apertura del fichero .ind (índice) */
    database->index_fp = fopen(indexf_name, "r+b");
    if (!(database->index_fp)) database->index_fp = fopen(indexf_name, "w+b");
    if (!(database->index_fp)) {
        fprintf(stderr, "Error opening or creating file: %s\n", indexf_name);
        fclose(database->data_fp);
        return ERR;
    }

    /* Apertura del fichero .lst (deletedlist) */
    database->deleted_fp = fopen(deletedf_name, "r+b");
    if (!(database->deleted_fp)) database->deleted_fp = fopen(deletedf_name, "w+b");
    if (!(database->deleted_fp)) {
        fprintf(stderr, "Error opening or creating file: %s\n", deletedf_name);
        fclose(database->data_fp);
        fclose(database->index_fp);
        return ERR;
    }

    /* Carga del índice desde el fichero */
    if (!(database->index = index_init_from_file(database->index_fp))) {
        fprintf(stderr, "Error loading index structure\n");
        fclose(database->data_fp);        
        fclose(database->index_fp);
        fclose(database->deleted_fp);
        return ERR;        
    }

    /* Carga de deletedlist desde el fichero */
    if (!(database->deletedlist = deletedlist_init_from_file(database->deleted_fp))) {
        fprintf(stderr, "Error loading deleted list structure\n");
        deletedlist_free(database->deletedlist);
        fclose(database->data_fp);
        fclose(database->index_fp);
        fclose(database->deleted_fp);
        return ERR;
    }

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Bucle principal de lectura y ejecución de comandos.
 *      Lee la entrada del usuario, analiza el comando, lo ejecuta y repite.
 * Parámetros:
 *      database - puntero a la estructura DBInfo con toda la información
 * Retorno:
 *      void
 ******************************************************************************/
void loop(DBInfo *database) 
{
    char user_input[MAX_INPUT];
    CommandCode current_command_code = NO_CMD;
    Book *book = NULL;

    if (dbinfo_error(database)) return; 

    fprintf(stdout, "Type command and argument/s.\n");
    fprintf(stdout, "exit\n");
    fflush(stdout);

    while (1) {
        int i = 0;
        int c = 0;

        /* inicialización del buffer */
        user_input[0] = '\0';

        /* lectura carácter a carácter */
        while ((c = fgetc(stdin)) != EOF) {
            if (c == '\n' || c == '\r') {
                user_input[i] = '\0';
                break;
            }
            if (i < MAX_INPUT - 1) {
                user_input[i++] = (char)c;
            }
        }

        if (c == EOF && i == 0) break; /* EOF y nada leído -> salir */

        if (i >= MAX_INPUT - 1) user_input[MAX_INPUT - 1] = '\0';

        if (strlen(user_input) == 0) continue; /* línea vacía -> siguiente */

        /* Parseo del comando */
        book = command_parse(user_input, &current_command_code);

        /* Ejecución del comando */
        command_execute(database->data_fp, database->index, database->index_fp, database->deletedlist, book, database->deleted_fp,
                        database->strategy, current_command_code, database->dbname);

        if (book){
            book_free(book);
        }

        /* imprimir exit solo si no es el último comando */
        if (current_command_code != EXIT) {
            fprintf(stdout, "exit\n");
            fflush(stdout);
        } else {
            fflush(stdout);
            break;
        }

    }
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Limpia y libera todos los recursos asociados a la base de datos.
 * Parámetros:
 *      database - puntero doble a DBInfo que se liberará
 * Retorno:
 *      void
 ******************************************************************************/
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
        if ((*database)->deletedlist) 
        { 
            deletedlist_free((*database)->deletedlist); 
            (*database)->deletedlist = NULL; 
        } 
        if ((*database)->index) 
        { 
            index_free((*database)->index); 
            (*database)->index = NULL; 
        }
        free(*database);
        *database = NULL;
    }
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Comprueba si la estructura DBInfo es válida y está correctamente
 *      inicializada.
 * Parámetros:
 *      db - puntero a DBInfo a comprobar
 * Retorno:
 *      true  - si hay error
 *      false - si todo es correcto
 ******************************************************************************/
Bool dbinfo_error(DBInfo *db)
{
    if (!db) return true;
    if (!db->data_fp) return true;
    if (!db->deleted_fp) return true;
    if (!db->index_fp) return true;
    if (!db->index) return true;
    if (!db->deletedlist) return true;
    if (db->strategy != BESTFIT && db->strategy != WORSTFIT && db->strategy != FIRSTFIT) return true;

    return false;
}