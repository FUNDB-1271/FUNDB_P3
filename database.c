/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Implementación de funciones de acceso a los registros en el fichero
 *      de la base de datos (.db). Permite añadir, eliminar, buscar y 
 *      mostrar registros, así como actualizar su tamaño.
 ******************************************************************************/

#include "database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Añade un nuevo registro al fichero de la base de datos.
 *      Calcula la longitud total del registro y lo escribe en el fichero,
 *      respetando el formato: total_length|book_id|isbn|title|publisher
 * Parámetros:
 *      database_f - puntero al fichero de la base de datos
 *      offset     - posición donde escribir el registro, NO_POS para añadir al final
 *      book_id    - identificador único del libro
 *      title      - título del libro
 *      isbn       - ISBN del libro
 *      publisher  - editorial del libro
 * Retorno:
 *      Posición en el fichero donde se escribió el registro o ERR en caso de error
 ******************************************************************************/
long database_add(FILE *database_f, long offset, int book_id, const char *title, const char *isbn, const char *publisher) {
    size_t title_length, publisher_length, total_length;
    char isbn_cpy[MAX_ISBN], a = '|';
    long chosen_pos, file_size, write_pos;
    size_t copy_length = 0;

    if (!database_f || !title || !isbn || !publisher) return ERR;

    /* Obtener tamaño actual del fichero */
    if (fseek(database_f, 0, SEEK_END) != 0) return ERR;
    file_size = ftell(database_f);

    /* Decidir posición de escritura: append o reutilizar offset */
    if (offset == NO_POS) {
        chosen_pos = file_size; /* agregar al final */
    } else {
        chosen_pos = offset; /* reutilizar espacio eliminado */
    }

    /* Guardar posición real de escritura */
    write_pos = chosen_pos;

    /* Calcular longitud de los campos respetando máximos */
    title_length = strlen(title) < MAX_TITLE ? strlen(title) : MAX_TITLE;
    publisher_length = strlen(publisher) < MAX_PUBLISHEDBY ? strlen(publisher) : MAX_PUBLISHEDBY;

    total_length = sizeof(int) + MAX_ISBN + title_length + 1 + publisher_length;

    /* Preparar ISBN truncado o rellenado con ceros */
    copy_length = strlen(isbn);    
    if (copy_length > MAX_ISBN) copy_length = MAX_ISBN;
    memset(isbn_cpy, '\0', MAX_ISBN);
    memcpy(isbn_cpy, isbn, copy_length);

    /* Posicionar puntero del fichero */
    if (fseek(database_f, chosen_pos, SEEK_SET) != 0) return ERR;

    /* Escribir registro completo */
    if (fwrite(&total_length, sizeof(size_t), 1, database_f) != 1) return ERR;
    if (fwrite(&book_id, sizeof(int), 1, database_f) != 1) return ERR;
    if (fwrite(isbn_cpy, sizeof(char), MAX_ISBN, database_f) != MAX_ISBN) return ERR;
    if (fwrite(title, sizeof(char), title_length, database_f) != title_length) return ERR;
    if (fwrite(&a, sizeof(char), 1, database_f) != 1) return ERR;
    if (fwrite(publisher, sizeof(char), publisher_length, database_f) != publisher_length) return ERR;

    fflush(database_f);
    return write_pos;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Marca un registro como eliminado en la base de datos y actualiza 
 *      las estructuras Index y DeletedList.
 * Parámetros:
 *      database_f  - puntero al fichero de la base de datos
 *      ind         - puntero al índice
 *      deletedList - puntero a la lista de huecos eliminados
 *      indexbook   - registro de índice del libro a eliminar
 * Retorno:
 *      OK si se realiza correctamente, ERR en caso de error
 ******************************************************************************/
int database_del(FILE *database_f, Index *ind, DeletedList *deletedList, IndexBook *indexbook){
    long offset;
    size_t size;
    
    if (!ind || !deletedList || !indexbook){
        return ERR;
    } 

    /* Obtener offset y tamaño del registro */
    offset = indexbook_get_offset(indexbook);
    size   = indexbook_get_size(indexbook);
    
    if (offset < 0 || size == 0)
        return ERR;

    /* Posicionar al inicio del registro */
    if (fseek(database_f, offset, SEEK_SET) != 0) {
        return ERR;
    }

    fflush(database_f);

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Busca un registro por su book_id e imprime su información completa
 *      en formato: book_id|isbn|title|publisher
 * Parámetros:
 *      fdb  - puntero al fichero de la base de datos
 *      ind  - puntero al índice
 *      key  - book_id a buscar
 * Retorno:
 *      OK si se encuentra y muestra el registro, NOT_FOUND si no existe,
 *      ERR en caso de error
 ******************************************************************************/
int database_find(FILE *fdb, Index *ind, int key){
    int pos;
    size_t total_length;
    int book_id;
    char isbn[MAX_ISBN + 1];
    char *title_ptr, *publisher_ptr;
    char *buffer;
    char *separator_ptr;

    if (fdb == NULL || ind == NULL || key < 0){
        return ERR;
    }

    pos = index_find(ind, key);
    if (pos == ERR){
        return ERR;
    }
    else if (pos == NOT_FOUND){
        printf("Record with bookId=%d does not exist\n", key);
        return NOT_FOUND;
    }

    /* Posicionar al registro en el fichero */
    if (fseek(fdb, indexbook_get_offset(index_get_indexbook(ind, pos)), SEEK_SET) != 0){
        return ERR;
    }

    /* Leer tamaño del registro */
    if (fread(&total_length, sizeof(size_t), 1, fdb) != 1)
        return ERR;
    
    /* Leer book_id */
    if (fread(&book_id, sizeof(int), 1, fdb) != 1)
        return ERR;
    
    /* Leer ISBN */
    if (fread(isbn, sizeof(char), MAX_ISBN, fdb) != MAX_ISBN)
        return ERR;
    isbn[MAX_ISBN] = '\0';
    
    /* Leer título y editorial */
    size_t remaining_size = total_length - sizeof(int) - MAX_ISBN;
    buffer = malloc(remaining_size + 1);
    if (buffer == NULL){
        return ERR;
    }
    
    if (fread(buffer, sizeof(char), remaining_size, fdb) != remaining_size){
        free(buffer);
        return ERR;
    }
    buffer[remaining_size] = '\0';
    
    /* Separar título y editorial */
    separator_ptr = memchr(buffer, '|', remaining_size);
    if (separator_ptr == NULL) {
        free(buffer);
        return ERR;
    }
    
    *separator_ptr = '\0';
    title_ptr = buffer;
    publisher_ptr = separator_ptr + 1;
    
    /* Imprimir registro */
    printf("%d|%s|%s|%s\n", book_id, isbn, title_ptr, publisher_ptr);
    
    free(buffer);
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Función de salida de la base de datos.
 * Retorno:
 *      0
 ******************************************************************************/
int database_exit(){
    return 0;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Imprime todos los registros almacenados en la base de datos.
 * Parámetros:
 *      database_f - puntero al fichero de la base de datos
 *      index      - puntero al índice
 * Retorno:
 *      OK si imprime todos los registros, ERR en caso de error
 ******************************************************************************/
int database_print_rec(FILE *database_f, Index *index){
    size_t i;
    size_t total_length;
    int book_id;
    char isbn[MAX_ISBN + 1];
    char *buffer;
    char *separator_ptr;
    IndexBook *indexbook;

    if (database_f == NULL || index == NULL){
        return ERR;
    }

    for (i = 0; i < index_get_used(index); i++){
        indexbook = index_get_indexbook(index, i);
        if (indexbook == NULL) continue;

        /* Posicionar al registro en el fichero */
        if (fseek(database_f, indexbook_get_offset(indexbook), SEEK_SET) != 0) return ERR;

        /* Leer tamaño del registro */
        if (fread(&total_length, sizeof(size_t), 1, database_f) != 1) return ERR;
        
        /* Leer book_id */
        if (fread(&book_id, sizeof(int), 1, database_f) != 1) return ERR;
        
        /* Leer ISBN */
        if (fread(isbn, sizeof(char), MAX_ISBN, database_f) != MAX_ISBN) return ERR;
        isbn[MAX_ISBN] = '\0';
        
        /* Leer título y editorial */
        size_t remaining_size = total_length - sizeof(int) - MAX_ISBN;
        buffer = malloc(remaining_size + 1);
        if (buffer == NULL) return ERR;
        
        if (fread(buffer, sizeof(char), remaining_size, database_f) != remaining_size){
            free(buffer);
            return ERR;
        }
        buffer[remaining_size] = '\0';
        
        /* Separar título y editorial */
        separator_ptr = memchr(buffer, '|', remaining_size);
        if (separator_ptr == NULL) {
            free(buffer);
            return ERR;
        }
        *separator_ptr = '\0';
        char *title_ptr = buffer;
        char *publisher_ptr = separator_ptr + 1;
        
        printf("%d|%s|%s|%s\n", book_id, isbn, title_ptr, publisher_ptr);
        
        free(buffer);
    }

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Actualiza el tamaño de un registro en la base de datos en la posición
 *      indicada.
 * Parámetros:
 *      fdb    - puntero al fichero de la base de datos
 *      size   - nuevo tamaño del registro
 *      offset - posición del registro a actualizar
 * Retorno:
 *      OK si se actualiza correctamente, ERR en caso de error
 ******************************************************************************/
int database_print_size(FILE *fdb, size_t size, long offset)
{
    long aux = 0;

    if (!fdb || offset < 0) return ERR;

    if ((aux = (long) fseek(fdb, 0, SEEK_END)) != 0) return ERR;

    if (offset > aux) return ERR;

    if ((aux = (long) fseek(fdb, 0, offset) != 0)) return ERR;

    if (fwrite(&size, sizeof(size_t), 1, fdb) != 1) return ERR;

    return OK;
}
