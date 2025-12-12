/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cabecera para la estructura Book y sus funciones asociadas.
 ******************************************************************************/

#ifndef BOOK_H
#define BOOK_H

#include "index.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct _book Book;

/* Inicialización y liberación de memoria */
Book *book_init(int book_id);
void book_free(Book *book);

/* Acceso y modificación de campos */
int book_get_id(Book *book);
int book_set_id(Book *book, int book_id);

const char* book_get_isbn(Book *book);
int book_set_isbn(Book *book, const char *isbn);

const char* book_get_title(Book *book);
int book_set_title(Book *book, const char *title);

const char* book_get_publishedby(Book *book);
int book_set_publishedby(Book *book, const char *publisher);

size_t book_get_total_size(Book *book);
int book_set_total_size(Book *book, size_t size);

#endif