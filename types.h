#ifndef TYPES_H
#define TYPES_H

#define NAME_MAX 32

#define ERR -1
#define OK !(ERR)

#define NO_POS -2
#define NOT_FOUND -3

#define MAX_INPUT 512

#define MAX_TITLE 128
#define MAX_PUBLISHEDBY 128
#define MAX_ISBN 16 

typedef enum {false, true} Bool;

typedef enum {NoError, UknError, BookExists, WriteError, MemError} AddErrors;

#endif