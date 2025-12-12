/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cabecera de funciones para la gestión de la lista de registros eliminados
 *      de la base de datos.
 ******************************************************************************/

#ifndef DELETEDLIST_H
#define DELETEDLIST_H

#include "index.h"

#define DELETEDLIST_INIT_SIZE 4

#include <stdio.h>

typedef struct _indexdeletedbook IndexDeletedBook;
typedef struct _deletedlist DeletedList;

/* Inicializa la estructura DeletedList */
DeletedList *deletedlist_init();

/* Libera memoria ocupada por la estructura DeletedList */
void deletedlist_free(DeletedList *deletedlist);

/* Devuelve offset de un hueco eliminado en la posición pos */
size_t deletedlist_get_offset(DeletedList *deletedlist, int pos);

/* Devuelve tamaño de registro de un hueco eliminado en la posición pos */
size_t deletedlist_get_size(DeletedList *deletedlist, int pos);

/* Añade un registro eliminado a la lista usando la estrategia indicada */
int deletedlist_add(DeletedList *deletedlist, IndexBook *indexbook, int strategy);

/* Actualiza la lista de eliminados según la operación ADD o DEL */
int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command_code);

/* Guarda la lista de eliminados en un archivo */
int deletedlist_save(DeletedList *deletedlist, char *file, int strategy);

/* Busca un hueco adecuado según la estrategia */
int deletedlist_find(DeletedList *deletedlist, size_t book_size, int strategy);

/* Inicializa la lista de eliminados a partir de un archivo */
DeletedList *deletedlist_init_from_file(FILE *deletedlist_fp);

/* Imprime el contenido de la lista de eliminados */
void deletedlist_print(DeletedList *deletedList);

#endif
