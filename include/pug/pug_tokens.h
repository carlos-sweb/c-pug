#ifndef PUG_TOKENS_H
#define PUG_TOKENS_H

#include <glib.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// ENUMERACIONES PARA TIPOS DE TOKENS
// ============================================================================
typedef enum :int{
    TOKEN_TAG = 0,          // div, p, span, etc.
    TOKEN_CLASS,           // .container, .menu
    TOKEN_ID,              // #main, #header
    TOKEN_ATTRIBUTE,       // (name="value")
    TOKEN_TEXT,            // Texto plano
    TOKEN_INDENTATION,     // Tabs o espacios (nivel jerárquico)
    TOKEN_DOT,             // . para bloques de texto
    TOKEN_PIPE,            // | para texto multilínea
    TOKEN_COMMENT,         // // Comentario en línea
    TOKEN_BLOCK_COMMENT,   // //- Comentario que no se renderiza
    TOKEN_INTERPOLATION,   // #{variable}
    TOKEN_CODE,           // - var x = 10
    TOKEN_DOCTYPE,        // doctype html
    TOKEN_INCLUDE,        // include file.pug
    TOKEN_EXTENDS,        // extends layout.pug
    TOKEN_MIXIN,         // mixin nombre(args)
    TOKEN_CALL,          // +mixin(args)
    TOKEN_EOF,           // Fin del archivo
    TOKEN_COUNT          // Contador total de tokens (no es un token real)
} TokenType;

typedef enum :int{
    ATTR_STRING,    // name="value"
    ATTR_BOOLEAN,   // disabled, checked
    ATTR_EXPRESSION,// data-id=variable
    ATTR_CLASS,     // class="container"
    ATTR_ID         // id="main"
} AttributeType;

// ============================================================================
// ESTRUCTURAS
// ============================================================================
typedef struct {
    char* name;
    char* value;
    AttributeType type;
} Attribute;

typedef struct {
    Attribute** attributes;
    unsigned int count;
    unsigned int capacity;
} AttributeList;

typedef struct {
    TokenType type;
    char* value;
    unsigned int line;
    unsigned int column;
    unsigned int depth;
    AttributeList* attributes;
} Token;

typedef struct {
    Token** tokens;
    unsigned int count;
    unsigned int capacity;
} TokenList;

typedef struct ASTNode {
    char* tag;
    char* text_content;
    TokenType node_type;
    AttributeList* attributes;
    char** classes;
    unsigned int class_count;
    char* id;
    unsigned int depth;
    unsigned int line;
    unsigned int column;
    struct ASTNode* parent;
    struct ASTNode** children;
    unsigned int children_count;
    unsigned int children_capacity;
    unsigned int is_void;
    unsigned int is_inline;
    unsigned int is_block;
} ASTNode;

typedef struct {
    ASTNode** nodes;
    unsigned int count;
    unsigned int capacity;
} NodeList;

typedef struct {
    TokenList* tokens;
    unsigned int current_token;
    unsigned int current_depth;
    ASTNode* current_parent;
    ASTNode* root_node;
    unsigned int use_tabs;
    unsigned int tab_size;
    unsigned int strict_mode;
} ParserContext;

#ifdef __cplusplus
}
#endif

#endif // PUG_TOKENS_H