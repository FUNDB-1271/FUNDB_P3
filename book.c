/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Implementación de la estructura Book y sus funciones asociadas.
 *      Permite inicializar, liberar, obtener y establecer campos, así como
 *      imprimir la información de un libro.
 ******************************************************************************/

#include "book.h"

#include <string.h>

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Estructura interna que representa un libro.
 ******************************************************************************/
struct _book{
    int book_id;
    char isbn[MAX_ISBN + 1];
    char title[MAX_TITLE];
    char publishedby[MAX_PUBLISHEDBY];
    size_t total_size;
};

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Inicializa un Book con un book_id específico.
 * Parámetros:
 *      book_id - ID del libro
 * Retorno:
 *      Puntero a Book inicializado o NULL si hay error de memoria
 ******************************************************************************/
Book *book_init(int book_id){
    Book* new_book = (Book*)malloc(sizeof(Book));
    if (new_book == NULL){
        return NULL;
    }

    new_book->book_id = book_id;
    new_book->isbn[MAX_ISBN] = '\n';
    new_book->title[MAX_TITLE-1] = '\n';
    new_book->publishedby[MAX_PUBLISHEDBY-1] = '\n';
    
    return new_book;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Libera la memoria asociada a un Book.
 * Parámetros:
 *      book - puntero al Book
 ******************************************************************************/
void book_free(Book *book){
    if (book != NULL){
        free(book);
    }
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el ID del libro.
 * Parámetros:
 *      book - puntero al Book
 * Retorno:
 *      book_id o ERR si book == NULL
 ******************************************************************************/
int book_get_id(Book *book) {
    if (book == NULL) {
        return ERR;
    }
    return book->book_id;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el ID del libro.
 * Parámetros:
 *      book    - puntero al Book
 *      book_id - nuevo ID
 * Retorno:
 *      OK si se asigna correctamente, ERR si book == NULL
 ******************************************************************************/
int book_set_id(Book *book, int book_id) {
    if (book == NULL) {
        return ERR;
    }
    book->book_id = book_id;
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el ISBN del libro.
 * Parámetros:
 *      book - puntero al Book
 * Retorno:
 *      Puntero a ISBN o NULL si book == NULL
 ******************************************************************************/
const char* book_get_isbn(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->isbn;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el ISBN del libro.
 * Parámetros:
 *      book - puntero al Book
 *      isbn - cadena con el ISBN
 * Retorno:
 *      OK si se asigna correctamente, ERR si book o isbn == NULL
 ******************************************************************************/
int book_set_isbn(Book *book, const char *isbn) {
    if (book == NULL || isbn == NULL) {
        return ERR;
    }
    strncpy(book->isbn, isbn, MAX_ISBN);
    book->isbn[MAX_ISBN] = '\0';
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el título del libro.
 * Parámetros:
 *      book - puntero al Book
 * Retorno:
 *      Puntero al título o NULL si book == NULL
 ******************************************************************************/
const char* book_get_title(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->title;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el título del libro.
 * Parámetros:
 *      book  - puntero al Book
 *      title - cadena con el título
 * Retorno:
 *      OK si se asigna correctamente, ERR si book o title == NULL
 ******************************************************************************/
int book_set_title(Book *book, const char *title) {
    if (book == NULL || title == NULL) {
        return ERR;
    }
    strncpy(book->title, title, MAX_TITLE - 1);
    book->title[MAX_TITLE - 1] = '\0';
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el campo publishedby del libro.
 * Parámetros:
 *      book - puntero al Book
 * Retorno:
 *      Puntero a publishedby o NULL si book == NULL
 ******************************************************************************/
const char* book_get_publishedby(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->publishedby;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el campo publishedby del libro.
 * Parámetros:
 *      book      - puntero al Book
 *      publisher - cadena con el publisher
 * Retorno:
 *      OK si se asigna correctamente, ERR si book o publisher == NULL
 ******************************************************************************/
int book_set_publishedby(Book *book, const char *publisher) {
    if (book == NULL || publisher == NULL) {
        return ERR;
    }
    strncpy(book->publishedby, publisher, MAX_PUBLISHEDBY - 1);
    book->publishedby[MAX_PUBLISHEDBY - 1] = '\0';
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el tamaño total del registro del Book.
 * Parámetros:
 *      book - puntero al Book
 * Retorno:
 *      total_size o 0 si book == NULL
 ******************************************************************************/
size_t book_get_total_size(Book *book) {
    if (book == NULL) {
        return 0;
    }
    return book->total_size;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el tamaño total del registro del Book.
 * Parámetros:
 *      book - puntero al Book
 *      size - tamaño a asignar
 * Retorno:
 *      OK si se asigna correctamente, ERR si book == NULL
 ******************************************************************************/
int book_set_total_size(Book *book, size_t size) {
    if (book == NULL) {
        return ERR;
    }
    book->total_size = size;
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Imprime un libro en formato: book_id|isbn|title|publishedby.
 * Parámetros:
 *      book - puntero al Book
 ******************************************************************************/
void book_print(Book *book){
    if (book == NULL){
        return;
    }

    printf("%d|%s|%s|%s\n", book->book_id, book->isbn, book->title, book->publishedby);
}