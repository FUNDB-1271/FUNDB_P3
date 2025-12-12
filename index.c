/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Implementación del módulo Index e IndexBook. Este módulo gestiona una
 *      estructura índice ordenada por claves, permitiendo insertar, eliminar,
 *      buscar y guardar entradas en fichero.
 ******************************************************************************/

#include "index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* used for memory-related functions */

struct _indexbook {
    int key; 
    long int offset;
    size_t size;
};

struct _index {
    IndexBook **books; /* pointers to books */
    size_t used;
    size_t size;
};

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Inicializa una estructura IndexBook reservando memoria y asignando valores.
 * Parámetros:
 *      key    → clave del libro
 *      offset → posición en fichero
 *      size   → tamaño total del registro
 * Retorno:
 *      Puntero a IndexBook creado o NULL si falla.
 ******************************************************************************/
IndexBook *indexbook_init(int key, long int offset, size_t size) {
    IndexBook *book = (IndexBook *) malloc(sizeof(IndexBook));
    if (book) {
        /* Asignación de campos */
        book->key = key;
        book->offset = offset;
        book->size = size;
    }
    return book;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Libera un IndexBook.
 * Parámetros:
 *      book → puntero a IndexBook
 * Retorno:
 *      Ninguno.
 ******************************************************************************/
void indexbook_free(IndexBook *book) {
    if (book) free(book);
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cambia el tamaño del IndexBook.
 * Parámetros:
 *      book → puntero al elemento
 *      size → nuevo tamaño
 * Retorno:
 *      OK si todo fue bien, ERR si book es NULL.
 ******************************************************************************/
int indexbook_set_size(IndexBook *book, size_t size){
    if (book == NULL){
        return ERR;
    }

    book->size = size;
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el tamaño del IndexBook.
 * Parámetros:
 *      book → elemento consultado
 * Retorno:
 *      Tamaño o 0 si book es NULL.
 ******************************************************************************/
size_t indexbook_get_size(IndexBook *book){
    if (book == NULL){
        return 0;
    }

    return book->size;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve la clave del IndexBook.
 * Parámetros:
 *      book → elemento consultado
 * Retorno:
 *      La clave del libro.
 ******************************************************************************/
int indexbook_get_key(IndexBook *book){
    return book->key;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Cambia el offset del libro.
 * Parámetros:
 *      book → puntero al IndexBook
 *      offset → nuevo offset
 * Retorno:
 *      OK si se pudo, ERR si book es NULL.
 ******************************************************************************/
int indexbook_set_offset(IndexBook *book, size_t offset){
    if (book == NULL){
        return ERR;
    }

    book->offset = offset;

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Obtiene el offset del libro.
 * Parámetros:
 *      book → elemento consultado
 * Retorno:
 *      offset o ERR si book es NULL.
 ******************************************************************************/
long int indexbook_get_offset(IndexBook *book){
    if (book == NULL){
        return ERR;
    }

    return book->offset;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Inicializa la estructura Index reservando espacio inicial para libros.
 * Parámetros:
 *      Ninguno.
 * Retorno:
 *      Index inicializado o NULL si falla.
 ******************************************************************************/
Index *index_init(void) {
    Index *ind = malloc(sizeof(Index));
    if (ind) {
        /* Reservamos memoria para el array dinámico */
        ind->books = (IndexBook **) malloc(INDEX_INIT_SIZE * sizeof(IndexBook *));
        if (!ind->books) {
            free(ind);
            return NULL;
        }
        ind->size = INDEX_INIT_SIZE;
        ind->used = 0;
    }

    return ind;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Libera toda la estructura Index y cada uno de sus elementos.
 * Parámetros:
 *      ind → índice a liberar
 * Retorno:
 *      Ninguno.
 ******************************************************************************/
void index_free(Index *ind) {
    size_t i;
    if (ind) 
    {
        if (ind->books) {
            /* Liberamos cada book alojado */
            for (i = 0 ; i < ind->used ; i++) 
                if (ind->books[i]) 
                    indexbook_free(ind->books[i]);

            free(ind->books);
        }
        ind->books = NULL;
        free(ind);
    }
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el IndexBook almacenado en la posición indicada.
 * Parámetros:
 *      ind → índice
 *      pos → posición
 * Retorno:
 *      Puntero al libro o NULL.
 ******************************************************************************/
IndexBook* index_get_indexbook(Index *ind, int pos){
    if (ind == NULL){
        return NULL;
    }

    return ind->books[pos];
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve cuántos elementos hay en el índice.
 * Parámetros:
 *      index → puntero a Index
 * Retorno:
 *      Número de elementos usados.
 ******************************************************************************/
size_t index_get_used(Index *index){
    if (index == NULL){
        return 0;
    }

    return index->used;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Añade un libro al índice en orden usando búsqueda binaria.
 *
 * PASOS DEL ALGORITMO:
 *      1. Comprobar parámetros.
 *      2. Redimensionar array si está lleno.
 *      3. Buscar posición mediante búsqueda binaria.
 *      4. Si existe la clave → devolver 1.
 *      5. Desplazar elementos a la derecha con memmove.
 *      6. Insertar book y aumentar used.
 *
 * Parámetros:
 *      ind  → índice
 *      book → elemento a insertar
 * Retorno:
 *      OK si se insertó, 1 si clave repetida, -1 si error.
 ******************************************************************************/
int index_add(Index *ind, IndexBook *book) {
    int m, a = 0, b = ind->used - 1;

    if (!ind || !ind->books || !book) return -1;

    /* Paso 2: redimensionar si está lleno */
    if (ind->size == ind->used)
    {
        size_t new_size = ind->size * 2;
        IndexBook **temp = realloc(ind->books, new_size * sizeof(IndexBook *));
        if (!temp) {
            return -1;
        }
        ind->books = temp;
        ind->size = new_size;
    }

    /* Paso 3: búsqueda binaria */
    while (a <= b) 
    {
        m = a + (b - a) / 2;

        if (book->key == ind->books[m]->key) {
            return 1; /* clave duplicada */
        } else if (book->key < ind->books[m]->key) {
            b = m - 1;
        } else {
            a = m + 1;
        } 
    }

    /* Paso 5: desplazar elementos */
    if (ind->used - a > 0) 
    {
        memmove(&(ind->books[a+1]), &(ind->books[a]), (ind->used - a) * sizeof(IndexBook *));
    }

    /* Paso 6: insertar */
    ind->books[a] = book;
    ind->used++;

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Guarda el índice en un fichero binario.
 *
 * PASOS DEL ALGORITMO:
 *      1. Abrir archivo temporal.
 *      2. Escribir entradas (key, offset, size).
 *      3. Cerrar y renombrar archivo temporal.
 *
 * Parámetros:
 *      ind → índice a guardar
 *      file → nombre del archivo destino
 * Retorno:
 *      0 si OK, -1 si error.
 ******************************************************************************/
int index_save(Index *ind, char *file) {
    size_t k = 0;
    FILE *temp = NULL;

    if (!ind || !ind->books || !file) return -1;

    if (!(temp = fopen("save.temp", "wb"))) return -1;

    for (k = 0 ; k < ind->used ; k++) {
        if (fwrite(&(ind->books[k]->key), sizeof(ind->books[k]->key), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k]->offset), sizeof(ind->books[k]->offset), 1, temp) != 1) goto write_fail;
        if (fwrite(&(ind->books[k]->size), sizeof(ind->books[k]->size), 1, temp) != 1) goto write_fail;
    }
    
    fflush(temp);
    fclose(temp);
    rename("save.temp", file);

    return 0;

write_fail:
    fclose(temp);
    return -1;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Carga un índice desde fichero previamente guardado.
 *
 * PASOS DEL ALGORITMO:
 *      1. Crear estructura Index.
 *      2. Leer entradas del fichero.
 *      3. Expandir array si es necesario.
 *      4. Crear IndexBook y añadirlo al índice.
 *
 * Parámetros:
 *      index_fp → puntero a archivo abierto en modo binario
 * Retorno:
 *      Estructura Index cargada o NULL si error.
 ******************************************************************************/
Index *index_init_from_file(FILE *index_fp) {
    Index *index = NULL;
    IndexBook *book = NULL;

    if (!index_fp) return NULL;

    if (!(index = (Index *) malloc(sizeof(Index)))) goto cleanup;

    if (!(index->books = (IndexBook **) malloc(INDEX_INIT_SIZE * sizeof(IndexBook *)))) goto cleanup;

    index->size = INDEX_INIT_SIZE;
    index->used = 0;         

    while (1) 
    {
        book = NULL;
        long offset; 
        size_t size;
        int key;

        /* Paso 3: redimensionar si está lleno */
        if (index->size == index->used) {
            index->size *= 2;
            index->books = realloc(index->books, index->size * sizeof(IndexBook *));
            if (!(index->books)) goto cleanup;
        }

        /* Paso 2: lectura binaria */
        if (fread(&key, sizeof(key), 1, index_fp) != 1) break;
        if (fread(&offset, sizeof(offset), 1, index_fp) != 1) goto cleanup;
        if (fread(&size, sizeof(size), 1, index_fp) != 1) goto cleanup;

        /* Paso 4: crear book */
        book = indexbook_init(key, offset, size);
        if (!book) goto cleanup;

        index->books[index->used++] = book;
    }

    fflush(index_fp);

    return index;

cleanup: 
    {
        if (book) indexbook_free(book);
        if (index) index_free(index);
    }
    return NULL;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Imprime todo el índice por pantalla.
 * Parámetros:
 *      ind → estructura a imprimir
 * Retorno:
 *      Ninguno.
 ******************************************************************************/
void index_print(Index *ind) {
    size_t i;
    if (!ind || !ind->books) return;
    for (i = 0 ; i < ind->used ; i++) {
        fprintf(stdout, "Entry #%d\n", (int)i);
        fprintf(stdout, "    key: #%d\n", ind->books[i]->key);
        fprintf(stdout, "    offset: #%ld\n", ind->books[i]->offset);
        fprintf(stdout, "    size: #%ld\n", ind->books[i]->size);
        fflush(stdout);
    }   
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Elimina un registro del índice manteniendo el orden.
 *
 * PASOS DEL ALGORITMO:
 *      1. Buscar la clave con búsqueda binaria (index_find).
 *      2. Liberar el IndexBook eliminado.
 *      3. Desplazar elementos.
 *
 * Parámetros:
 *      ind → índice
 *      key → clave a borrar
 * Retorno:
 *      OK si borrado, NOT_FOUND si no existe,
 *      ERR si error.
 ******************************************************************************/
int index_del(Index *ind, int key) {
    int m;

    if (!ind || !ind->books || ind->used == 0) return -1;

    m = index_find(ind, key);
    if (m == ERR){
        return ERR;
    }
    else if(m != NO_POS){
        indexbook_free(ind->books[m]);

        /* Paso 3: desplazar */
        if (m < (int) ind->used - 1) {
            memmove(&(ind->books[m]), &(ind->books[m+1]), (ind->used - m - 1) * sizeof(IndexBook *));
            ind->used--;
            ind->books[ind->used] = NULL;
        }
        else {
            ind->used--;
            ind->books[ind->used] = NULL;
        }
        return OK;
    }
    return NOT_FOUND;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Busca una clave en el índice mediante búsqueda binaria.
 * Parámetros:
 *      ind → índice
 *      key → clave a buscar
 * Retorno:
 *      Posición encontrada, NOT_FOUND si no existe, ERR si error.
 ******************************************************************************/
int index_find(Index *ind, int key){
    int a, b, m;

    if (ind == NULL || key < 0){
        return ERR;
    }

    a = 0;
    b = ind->used - 1;

    while (a <= b) 
    {
        m = a + (b - a) / 2;

        if (key == ind->books[m]->key) {
            return m;
        } else if (key < ind->books[m]->key) {
            b = m - 1;
        } else {
            a = m + 1;
        } 
    }
    
    return NOT_FOUND;
}
