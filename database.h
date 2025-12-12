/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cabecera de funciones de acceso y manipulación de la base de datos.
 ******************************************************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include "deletedlist.h"
#include "index.h"

/* Agregar un libro a la base de datos, devuelve posición escrita */
long database_add(FILE *database_f, long offset, int book_id, const char *title, const char *isbn, const char *publisher);

/* Buscar un libro por key e imprimir su contenido */
int database_find(FILE *fdb, Index *ind, int key);

/* Eliminar un libro de la base de datos y actualizar la lista de eliminados */
int database_del(FILE *database_f, Index *ind, DeletedList *deletedList, IndexBook *indexbook);

/* Escribir tamaño en un registro en la base de datos */
int database_print_size(FILE *fdb, size_t size, long offset);

/* Función para cerrar/terminar operaciones de la base de datos */
int database_exit();

/* Función para imprimir todos los registros de la base de datos */
int database_print_rec(FILE *database_f, Index *index);

#endif

