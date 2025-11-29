#include "book.h"

#include <string.h>

struct _book{
    int book_id;
    char isbn[MAX_ISBN + 1];
    char title[MAX_TITLE];
    char publishedby[MAX_PUBLISHEDBY];
    size_t total_size;
};

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

void book_free(Book *book){
    if (book != NULL){
        free(book);
    }
}

int book_get_id(Book *book) {
    if (book == NULL) {
        return ERR;
    }
    return book->book_id;
}

int book_set_id(Book *book, int book_id) {
    if (book == NULL) {
        return ERR;
    }
    book->book_id = book_id;
    return OK;
}

const char* book_get_isbn(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->isbn;
}

int book_set_isbn(Book *book, const char *isbn) {
    if (book == NULL || isbn == NULL) {
        return ERR;
    }
    strncpy(book->isbn, isbn, MAX_ISBN);
    book->isbn[MAX_ISBN] = '\0';
    return OK;
}

const char* book_get_title(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->title;
}

int book_set_title(Book *book, const char *title) {
    if (book == NULL || title == NULL) {
        return ERR;
    }
    strncpy(book->title, title, MAX_TITLE - 1);
    book->title[MAX_TITLE - 1] = '\0';
    return OK;
}

const char* book_get_publishedby(Book *book) {
    if (book == NULL) {
        return NULL;
    }
    return book->publishedby;
}

int book_set_publishedby(Book *book, const char *publisher) {
    if (book == NULL || publisher == NULL) {
        return ERR;
    }
    strncpy(book->publishedby, publisher, MAX_PUBLISHEDBY - 1);
    book->publishedby[MAX_PUBLISHEDBY - 1] = '\0';
    return OK;
}

size_t book_get_total_size(Book *book) {
    if (book == NULL) {
        return 0;
    }
    return book->total_size;
}

int book_set_total_size(Book *book, size_t size) {
    if (book == NULL) {
        return ERR;
    }
    book->total_size = size;
    return OK;
}

void book_print(Book *book){
    if (book == NULL){
        return;
    }

    printf("%d|%s|%s|%s\n", book->book_id, book->isbn, book->title, book->publishedby);
}



