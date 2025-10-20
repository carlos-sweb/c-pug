#ifndef PUG_TOKENS_LIST_H
#define PUG_TOKENS_LIST_H

#include <glib.h>
#include <stdlib.h>
#include "pug/pug_tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline TokenList* token_list_create() {
    //TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    TokenList* list = g_new(TokenList,1);
    if (!list) return NULL;    
    list->capacity = 10;
    list->count = 0;    
    //list->tokens = (Token**)malloc(sizeof(Token*) * list->capacity);    
    list->tokens = g_new(Token,list->capacity);
    if (!list->tokens) {
        free(list);
        return NULL;
    }
    return list;
}

static inline void token_list_add(TokenList* list, Token* token) {
    if (!list || !token) return;
    if (list->count >= list->capacity) {
        unsigned int new_capacity = list->capacity * 2;
        Token** new_tokens = (Token**)realloc(list->tokens, sizeof(Token*) * new_capacity);
        if (!new_tokens) {
            // Manejar error (por ejemplo, log o retorno)
            return;
        }
        list->tokens = new_tokens;
        list->capacity = new_capacity;
    }
    list->tokens[list->count] = token;
    list->count++;
}



static inline void token_list_free(TokenList* list) {
    if (!list) return;
    
    for (unsigned int i = 0; i < list->count; i++) {
        if (list->tokens[i]) {
            if (list->tokens[i]->value) {
                free(list->tokens[i]->value);
            }
            if (list->tokens[i]->attributes) {
                attribute_list_free(list->tokens[i]->attributes);
            }
            free(list->tokens[i]);
        }
    }
    
    free(list->tokens);
    free(list);
}

#ifdef __cplusplus
}
#endif

#endif // PUG_TOKENS_LIST_H