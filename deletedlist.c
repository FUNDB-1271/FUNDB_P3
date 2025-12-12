/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Implementación del módulo deletedlist e indexdeletedbook. 
 ******************************************************************************/

#include "deletedlist.h"
#include "commands.h"

#include <stdio.h>
#include <stdlib.h>

/* Estructura que almacena un hueco eliminado */
struct _indexdeletedbook {
    size_t register_size;
    size_t offset;
};

/* Lista dinámica de huecos eliminados */
struct _deletedlist {
    IndexDeletedBook *deleted;
    int used;
    int size;
};

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Inicializa una nueva DeletedList reservando memoria para la estructura
 *      principal y para el array interno donde se almacenarán los huecos.
 *      Se inicia la lista con tamaño DELETEDLIST_INIT_SIZE y con 0 elementos usados.
 * Parámetros:
 *      Ninguno.
 * Retorno:
 *      Puntero a la nueva DeletedList o NULL si hubo error reservando memoria.
 ******************************************************************************/
DeletedList *deletedlist_init() {
    DeletedList *d = malloc(sizeof(DeletedList));
    if (d == NULL){
        return NULL;
    }

    d->size = DELETEDLIST_INIT_SIZE;           
    d->used = 0;   

    /* Reserva inicial del array dinámico */
    d->deleted = malloc(d->size * sizeof(IndexDeletedBook));
    if (d->deleted == NULL){
        free(d);
        return NULL;
    }

    return d;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Libera toda la memoria asociada a una DeletedList, tanto la estructura
 *      como el array de huecos almacenados. No hace nada si recibe NULL.
 * Parámetros:
 *      deletedlist - puntero a la lista a liberar.
 * Retorno:
 *      Ninguno.
 ******************************************************************************/
void deletedlist_free(DeletedList *deletedlist){
    if (deletedlist != NULL){
        if (deletedlist->deleted != NULL){
            free(deletedlist->deleted);
        }
        free(deletedlist);
    }
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el offset del hueco que se encuentra en una posición dada.
 * Parámetros:
 *      deletedlist - lista de huecos eliminados.
 *      pos         - posición dentro del array.
 * Retorno:
 *      Offset almacenado o 0 si deletedlist == NULL.
 ******************************************************************************/
size_t deletedlist_get_offset(DeletedList *deletedlist, int pos){
    if (deletedlist == NULL){
        return 0;
    }    

    return deletedlist->deleted[pos].offset;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el tamaño de un IndexDeletedBook.
 * Parámetros:
 *      indexdeletedbook - puntero a la estructura objetivo.
 *      register_size    - valor a establecer.
 * Retorno:
 *      OK si se estableció correctamente, ERR si indexdeletedbook == NULL.
 ******************************************************************************/
int indexdeletedbook_set_register_size(IndexDeletedBook *indexdeletedbook, size_t register_size){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    indexdeletedbook->register_size = register_size;
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el tamaño de registro almacenado en un IndexDeletedBook.
 * Parámetros:
 *      indexdeletedbook - puntero al hueco.
 * Retorno:
 *      Tamaño del registro o ERR si indexdeletedbook == NULL.
 ******************************************************************************/
size_t indexdeletedbook_get_register_size(IndexDeletedBook *indexdeletedbook){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    return indexdeletedbook->register_size;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Establece el offset de un IndexDeletedBook.
 * Parámetros:
 *      indexdeletedbook - puntero al hueco.
 *      offset           - nuevo valor.
 * Retorno:
 *      OK si correcto, ERR si indexdeletedbook == NULL.
 ******************************************************************************/
int indexdeletedbook_set_offset(IndexDeletedBook *indexdeletedbook, size_t offset){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    indexdeletedbook->offset = offset;
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el offset almacenado en un IndexDeletedBook.
 * Parámetros:
 *      indexdeletedbook - puntero al hueco.
 * Retorno:
 *      Offset o ERR si indexdeletedbook == NULL.
 ******************************************************************************/
size_t indexdeletedbook_get_offset(IndexDeletedBook *indexdeletedbook){
    if (indexdeletedbook == NULL){
        return ERR;
    }

    return indexdeletedbook->offset;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Añade un hueco eliminado a la lista aplicando la estrategia de inserción:
 *          - FIRSTFIT: inserta al final sin ordenar
 *          - BESTFIT: inserta en posición ordenada ascendente por tamaño
 *          - WORSTFIT: inserta en posición descendente por tamaño
 *
 *      Si la lista está llena, la función la redimensiona usando realloc.
 *
 * Parámetros:
 *      deletedlist - lista donde insertar el hueco.
 *      indexbook   - estructura con tamaño y offset del hueco.
 *      strategy    - estrategia de inserción.
 * Retorno:
 *      OK si éxito, ERR si error o parámetros inválidos.
 ******************************************************************************/
int deletedlist_add(DeletedList *deletedlist, IndexBook *indexbook, int strategy){
    int i, j;

    /* Construcción del hueco a insertar */
    IndexDeletedBook deletedbook;
    deletedbook.register_size = indexbook_get_size(indexbook);
    deletedbook.offset = indexbook_get_offset(indexbook);

    if (deletedlist == NULL || indexbook == NULL){
        return -1;
    }

    /* Paso 1: Redimensionar si es necesario */
    if (deletedlist->size == deletedlist->used){
        deletedlist->size *= 2;
        if (deletedlist->size == 0){
            deletedlist->size = 10;
        }
        deletedlist->deleted = realloc(deletedlist->deleted, deletedlist->size * sizeof(IndexDeletedBook));
        if (deletedlist->deleted == NULL){
            return -1;
        }
    }
    else if(deletedlist->size < deletedlist->used){
        return -1;
    }

    /* Paso 2: Insertar según estrategia */
    if (strategy == FIRSTFIT){
        deletedlist->deleted[deletedlist->used++] = deletedbook;
        return OK;  
    }
    else if(strategy == BESTFIT){
        i = 0;
        while (i < deletedlist->used && deletedlist->deleted[i].register_size < deletedbook.register_size){
            i++;
        }
    }
    else if(strategy == WORSTFIT){
        i = 0;
        while (i < deletedlist->used && deletedlist->deleted[i].register_size > deletedbook.register_size){
            i++;
        }        
    }
    else{
        return ERR;
    }

    /* Paso 3: Desplazar elementos a la derecha para hacer hueco */
    for (j = deletedlist->used; j > i; j--) {
        deletedlist->deleted[j] = deletedlist->deleted[j-1];
    }

    /* Paso 4: Insertar el nuevo hueco */
    deletedlist->deleted[i] = deletedbook;
    deletedlist->used++;
    
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Busca un hueco que pueda reutilizarse, según la estrategia (FIRST/BEST/WORST),
 *      y lo actualiza o elimina dependiendo del espacio sobrante.
 *
 *      Implementación por pasos del algoritmo:
 *          1. Buscar hueco adecuado → deletedlist_find(...)
 *          2. Calcular espacio sobrante tras escribir nuevo registro
 *          3. Si el hueco no sirve (muy pequeño) → eliminarlo
 *          4. Si sobra espacio útil → actualizar hueco dividiéndolo
 *          5. Reordenar si la estrategia lo requiere
 *
 * Parámetros:
 *      deletedlist - lista de huecos existentes.
 *      indexbook   - registro nuevo cuyo tamaño hay que acomodar.
 *      strategy    - estrategia de elección de hueco.
 * Retorno:
 *      OK, ERR o NO_POS.
 ******************************************************************************/
int deletedlist_del(DeletedList *deletedlist, IndexBook *indexbook, int strategy){
    int i, pos;
    size_t remaining_space;
    size_t new_record_size;
    
    /* Mínimo tamaño útil de hueco */
    const size_t MIN_HOLE_SIZE = sizeof(size_t) + 1; 

    if (deletedlist == NULL || indexbook == NULL){
        return ERR;
    }

    /* Paso 1: Obtener tamaño del registro */
    new_record_size = indexbook_get_size(indexbook);

    /* Paso 2: Buscar hueco adecuado según estrategia */
    pos = deletedlist_find(deletedlist, new_record_size, strategy);
    if (pos == ERR){
        return ERR;
    }
    else if (pos == NO_POS || pos == NOT_FOUND){
        return NO_POS;
    }
    
    /* Paso 3: Calcular espacio sobrante tras insertar el nuevo registro */
    if (deletedlist->deleted[pos].register_size < (sizeof(size_t) + new_record_size)){
        remaining_space = 0; 
    } else {
        remaining_space = deletedlist->deleted[pos].register_size - (sizeof(size_t) + new_record_size);
    }
    
    /* Paso 4: Si el hueco ya no es útil → eliminarlo */
    if (remaining_space < MIN_HOLE_SIZE) {
        for (i = pos; i < deletedlist->used-1; i++){
            deletedlist->deleted[i] = deletedlist->deleted[i+1];
        }
        deletedlist->used--;
        return OK;
    }
    else { 
        /* Paso 5: Actualizar hueco desplazando offset y reduciendo tamaño */
        deletedlist->deleted[pos].offset += sizeof(size_t) + new_record_size;
        deletedlist->deleted[pos].register_size = remaining_space;
        
        /* Reordenar si aplica */
        if (strategy != FIRSTFIT) {
            IndexDeletedBook temp = deletedlist->deleted[pos];
            
            for (i = pos; i < deletedlist->used - 1; i++){
                deletedlist->deleted[i] = deletedlist->deleted[i+1];
            }
            
            int new_pos = 0;
            if (strategy == BESTFIT){
                while (new_pos < deletedlist->used - 1 && 
                       deletedlist->deleted[new_pos].register_size < temp.register_size){
                    new_pos++;
                }
            }
            else if (strategy == WORSTFIT){
                while (new_pos < deletedlist->used - 1 && 
                       deletedlist->deleted[new_pos].register_size > temp.register_size){
                    new_pos++;
                }
            }
            
            for (i = deletedlist->used - 1; i > new_pos; i--){
                deletedlist->deleted[i] = deletedlist->deleted[i-1];
            }
            
            deletedlist->deleted[new_pos] = temp;
        }
        
        return OK;
    }

    return ERR;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Decide si se debe añadir (DEL) o eliminar/reutilizar (ADD) un hueco.
 * Parámetros:
 *      deletedlist - lista que gestionar.
 *      indexbook   - registro relacionado con la operación.
 *      strategy    - estrategia FIRST/BEST/WORST.
 *      command_code- ADD o DEL.
 * Retorno:
 *      OK o ERR.
 ******************************************************************************/
int deletedlist_update(DeletedList *deletedlist, IndexBook *indexbook, int strategy, int command_code){
    if (deletedlist == NULL || command_code == NO_CMD){
        return ERR;
    }
    
    if(command_code == ADD){
        /* Intentar reutilizar un hueco */
        if (deletedlist_del(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    else if(command_code == DEL){
        /* Registrar un espacio eliminado */
        if (deletedlist_add(deletedlist, indexbook, strategy) == ERR){
            return ERR;
        }
    }
    
    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Encuentra hueco más pequeño que sirva (BESTFIT).
 * Parámetros:
 *      deletedlist - lista de huecos.
 *      book_size   - tamaño del registro nuevo.
 * Retorno:
 *      Posición del mejor hueco o NO_POS/ERR.
 ******************************************************************************/
int deletedlist_findbestfit(DeletedList *deletedlist, size_t book_size){
    int i, pos = NO_POS;
    size_t min = 0;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return ERR;
    }

    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
            if (min > deletedlist->deleted[i].register_size || min == 0){
                pos = i;
                min = deletedlist->deleted[i].register_size;
            }
        }
    }

    return pos;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Encuentra el hueco más grande adecuado (WORSTFIT).
 * Parámetros:
 *      deletedlist - lista de huecos.
 *      book_size   - tamaño necesario.
 * Retorno:
 *      Posición o NO_POS/ERR.
 ******************************************************************************/
int deletedlist_findworstfit(DeletedList *deletedlist, size_t book_size){
    int i, pos = NO_POS;
    size_t max = 0;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return -1;
    }
    
    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
            if (max < deletedlist->deleted[i].register_size || max == 0){
                pos = i;
                max = deletedlist->deleted[i].register_size;
            }
        }
    }

    return pos;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el primer hueco adecuado encontrado (FIRSTFIT).
 * Parámetros:
 *      deletedlist - lista de huecos.
 *      book_size   - tamaño requerido.
 * Retorno:
 *      Posición o NO_POS.
 ******************************************************************************/
int deletedlist_findfirstfit(DeletedList *deletedlist, size_t book_size){
    int i;

    if (deletedlist == NULL || deletedlist->deleted == NULL){
        return ERR;
    }
    
    size_t required_size = book_size + sizeof(size_t); 

    for (i = 0; i < deletedlist->used; i++){
        if (deletedlist->deleted[i].register_size >= required_size){
            return i;
        }
    }

    return NO_POS;
}   

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Redirige la búsqueda del hueco según la estrategia indicada.
 * Parámetros:
 *      deletedlist - lista de huecos.
 *      book_size   - tamaño requerido.
 *      strategy    - FIRSTFIT / BESTFIT / WORSTFIT.
 * Retorno:
 *      Posición o ERR.
 ******************************************************************************/
int deletedlist_find(DeletedList *deletedlist, size_t book_size, int strategy){
    if (deletedlist == NULL || (strategy != FIRSTFIT && strategy != WORSTFIT && strategy != BESTFIT)){
        return ERR;
    }

    if (strategy == BESTFIT){
        return deletedlist_findbestfit(deletedlist, book_size);
    }
    else if (strategy == WORSTFIT){
        return deletedlist_findworstfit(deletedlist, book_size);
    }
    else if (strategy == FIRSTFIT){
        return deletedlist_findfirstfit(deletedlist, book_size);
    }

    return ERR;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Guarda en fichero binario la lista de huecos y la estrategia usada.
 * Parámetros:
 *      deletedlist - lista a guardar.
 *      file        - ruta del archivo.
 *      strategy    - estrategia utilizada.
 * Retorno:
 *      OK si éxito, ERR si error.
 ******************************************************************************/
int deletedlist_save(DeletedList *deletedlist, char *file, int strategy) {
    int k = 0;
    FILE *temp = NULL;

    if (!deletedlist || !deletedlist->deleted || !file) return ERR;

    if (!(temp = fopen(file, "wb"))) return -1;

    /* Guardar cabecera con estrategia */
    if (fwrite(&strategy, sizeof(strategy), 1, temp) != 1) {
        fclose(temp);
        return ERR;
    }

    /* Guardar cada hueco */
    for (k = 0 ; k < deletedlist->used ; k++) {
        if (fwrite(&(deletedlist->deleted[k].offset), sizeof(deletedlist->deleted[k].offset), 1, temp) != 1){
            fclose (temp);
            return ERR;
        }
        if (fwrite(&(deletedlist->deleted[k].register_size), sizeof(deletedlist->deleted[k].register_size), 1, temp) != 1){
            fclose (temp);
            return ERR;
        }
    }
    
    fflush(temp);
    fclose(temp);

    return OK;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Carga desde fichero la lista de huecos eliminados previamente guardada.
 *      Reconstruye dinámicamente la DeletedList y rellena su array interno.
 * Parámetros:
 *      deletedlist_fp - fichero abierto en modo lectura binaria.
 * Retorno:
 *      Nueva DeletedList cargada, o NULL si error.
 ******************************************************************************/
DeletedList *deletedlist_init_from_file(FILE *deletedlist_fp) {
    DeletedList *deletedlist = NULL;
    int strategy_header; 

    if (!deletedlist_fp) return NULL;

    /* Leer estrategia almacenada */
    if (fread(&strategy_header, sizeof(int), 1, deletedlist_fp) != 1) {
        if (feof(deletedlist_fp)) {
            return deletedlist_init();
        }
        return NULL;
    }

    deletedlist = deletedlist_init();
    if (deletedlist == NULL){
        return NULL;
    }

    while (1) 
    {
        long int offset; 
        size_t register_size;

        /* Leer offset */
        if (fread(&offset, sizeof(offset), 1, deletedlist_fp) != 1) {
            if (feof(deletedlist_fp)) break;
            deletedlist_free(deletedlist);
            return NULL;
        }

        /* Leer tamaño */
        if (fread(&register_size, sizeof(register_size), 1, deletedlist_fp) != 1) {
            deletedlist_free(deletedlist);
            return NULL;
        }

        /* Redimensionar si necesario */
        if (deletedlist->size == deletedlist->used) {
            int new_size = deletedlist->size * 2;
            if (new_size == 0) new_size = DELETEDLIST_INIT_SIZE;
            IndexDeletedBook *tmp = realloc(deletedlist->deleted, new_size * sizeof(IndexDeletedBook));
            if (tmp == NULL) {
                deletedlist_free(deletedlist);
                return NULL;
            }
            deletedlist->deleted = tmp;
            deletedlist->size = new_size;
        }

        /* Guardar datos en la lista */
        if (indexdeletedbook_set_offset(&deletedlist->deleted[deletedlist->used], offset) == ERR){
            deletedlist_free(deletedlist);
            return NULL;
        }

        if (indexdeletedbook_set_register_size(&deletedlist->deleted[deletedlist->used], register_size) == ERR){
            deletedlist_free(deletedlist);
            return NULL;
        }
        deletedlist->used++;
    }

    return deletedlist;
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Imprime por pantalla cada uno de los huecos de la lista.
 * Parámetros:
 *      deletedList - lista a imprimir.
 * Retorno:
 *      Ninguno.
 ******************************************************************************/
void deletedlist_print(DeletedList *deletedList) {
    int i;
    if (deletedList == NULL || deletedList->deleted == NULL) return;

    for (i = 0 ; i < deletedList->used ; i++) {
        fprintf(stdout, "Entry #%d\n", i);
        fprintf(stdout, "    offset: #%ld\n", deletedList->deleted[i].offset);
        fprintf(stdout, "    size: #%ld\n", deletedList->deleted[i].register_size);        
    }   
}

/******************************************************************************
 * Autores: Marco Manceñido y Guilherme Povedano
 * Descripción:
 *      Devuelve el tamaño del hueco en una posición dada.
 * Parámetros:
 *      deletedlist - lista de huecos.
 *      pos         - índice del hueco.
 * Retorno:
 *      Tamaño o 0 si deletedlist == NULL.
 ******************************************************************************/
size_t deletedlist_get_size(DeletedList *deletedlist, int pos)
{
    if (deletedlist == NULL){
        return 0;
    }    

    return deletedlist->deleted[pos].register_size;
}