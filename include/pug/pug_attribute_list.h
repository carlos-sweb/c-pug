#ifndef PUG_ATTRIBUTE_LIST_H
#define PUG_ATTRIBUTE_LIST_H

#include <glib.h>
#include <stdlib.h>
#include "pug/pug_tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// IMPLEMENTACIÓN INLINE - AttributeList
// ============================================================================
static inline AttributeList* attribute_list_create() {
    //AttributeList* list = (AttributeList*)malloc(sizeof(AttributeList));
    AttributeList* list = g_new(AttributeList,1);
    if (!list) return NULL;

    list->capacity = 5;
    list->count = 0;
    //list->attributes = (Attribute**)malloc(sizeof(Attribute*) * list->capacity);
    list->attributes = g_new(Attribute,list->capacity);
    
    if (!list->attributes) {
        free(list);
        return NULL;
    }

    return list;
}

static inline void attribute_list_add(AttributeList* list, Attribute* attr) {
    if (!list || !attr) return;    
    if (list->count >= list->capacity) {
        unsigned int new_capacity = list->capacity * 2;
        Attribute** new_attrs = (Attribute**)realloc(list->attributes, sizeof(Attribute*) * new_capacity);
        if (!new_attrs) return;
        list->capacity = new_capacity;
        list->attributes = new_attrs;
    }    
    list->attributes[list->count] = attr;
    list->count++;
}

static inline void attribute_list_free(AttributeList* list) {
    if (!list) return;
    
    for (unsigned int i = 0; i < list->count; i++) {
        if (list->attributes[i]) {
            if (list->attributes[i]->name) {
                free(list->attributes[i]->name);
            }
            if (list->attributes[i]->value) {
                free(list->attributes[i]->value);
            }
            free(list->attributes[i]);
        }
    }
    
    free(list->attributes);
    free(list);
}


#ifdef __cplusplus
}
#endif

#endif // PUG_ATTRIBUTE_LIST_H