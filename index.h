/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cabecera de funciones para la gestión del índice de libros en la base 
 *      de datos.
 ******************************************************************************/

#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include "types.h"

#define INDEX_INIT_SIZE 4
#define BESTFIT 0
#define WORSTFIT 1
#define FIRSTFIT 2

typedef struct _indexbook IndexBook;
typedef struct _index Index;

/* Inicializa un IndexBook con key, offset y tamaño */
IndexBook *indexbook_init(int key, long int offset, size_t size);

/* Libera memoria ocupada por un IndexBook */
void indexbook_free(IndexBook *book);

/* Establece el tamaño de registro de un IndexBook */
int indexbook_set_size(IndexBook *book, size_t size);

/* Devuelve el tamaño de registro de un IndexBook */
size_t indexbook_get_size(IndexBook *book);

/* Devuelve la clave de un IndexBook */
int indexbook_get_key(IndexBook *book);

/* Establece el offset de un IndexBook */
int indexbook_set_offset(IndexBook *book, size_t offset);

/* Devuelve el offset de un IndexBook */
long int indexbook_get_offset(IndexBook *book);

/* Inicializa la estructura Index */
Index *index_init(void);

/* Libera memoria ocupada por la estructura Index */
void index_free(Index *ind);

/* Devuelve el puntero al IndexBook en la posición pos */
IndexBook* index_get_indexbook(Index *ind, int pos);

/* Devuelve el número de libros en el índice */
size_t index_get_used(Index *Index);

/* Añade un IndexBook al índice manteniendo orden */
int index_add(Index *ind, IndexBook *book);

/* Guarda el índice en un archivo */
int index_save(Index *ind, char *file);

/* Inicializa el índice desde un archivo */
Index *index_init_from_file(FILE *index_fp);

/* Imprime el contenido del índice */
void index_print(Index *ind);

/* Elimina un libro del índice por key */
int index_del(Index *ind, int key);

/* Busca un libro en el índice por key */
int index_find(Index *ind, int key);

#endif