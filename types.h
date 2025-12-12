/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cabecera con tipos, constantes y enumeraciones usadas en la base de datos.
 ******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#define NAME_MAX 32

#define OK 0
#define ERR -1
#define NO_POS -2
#define NOT_FOUND -3

#define MAX_INPUT 512

#define MAX_TITLE 128
#define MAX_PUBLISHEDBY 128
#define MAX_ISBN 16

#define SIZE_T_SIZE 8
#define MIN_REG_SIZE SIZE_T_SIZE + 1

typedef enum {false, true} Bool;

typedef enum {NoError = 1, UknError, BookExists, WriteError, MemError} AddErrors;

#endif
