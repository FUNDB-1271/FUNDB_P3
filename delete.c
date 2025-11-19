#include "delete.h"

#include <stdio.h>

struct _indexdeletedbook {
    size_t register_size;
    size_t offset;
};

struct _deletedlist {
    IndexDeletedBook *deleted;
    size_t used;
    size_t size;
};

DeletedList * deletedlist_init(){
    DeletedList * newDeletedList = (DeletedList*)malloc(sizeof(DeletedList));
    if(newDeletedList == NULL){
        return NULL;
    }
}

void deletedlist_free(DeletedList *deletedlist){
    if (deletedlist != NULL){
        free(deletedlist);
    }
}

int deletedlist_add(DeletedList *deletedlist, IndexDeletedBook *indexdeletedbook){
    


}


