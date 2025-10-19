#ifndef C_PUG_TOKENS_H
#define C_PUG_TOKENS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// ENUMERACIONES PARA TIPOS DE TOKENS
// ============================================================================
typedef enum {
    TOKEN_TAG,              // div, p, span, etc.
    TOKEN_CLASS,            // .container, .menu
    TOKEN_ID,               // #main, #header
    TOKEN_ATTRIBUTE,        // (name="value")
    TOKEN_TEXT,             // Texto plano
    TOKEN_INDENTATION,      // Tabs o espacios
    TOKEN_DOT,              // . para bloques de texto
    TOKEN_PIPE,             // | para texto multilínea
    TOKEN_COMMENT,          // // Comentario
    TOKEN_INTERPOLATION,    // #{variable}
    TOKEN_CODE,             // - var x = 10
    TOKEN_DOCTYPE,          // doctype html
    TOKEN_INCLUDE,          // include file.pug
    TOKEN_EXTENDS,          // extends layout.pug
    TOKEN_BLOCK,            // block content
    TOKEN_MIXIN,            // mixin nombre
    TOKEN_EOF               // Fin de archivo
} TokenType;

typedef enum {
    ATTR_STRING,    // name="value"
    ATTR_BOOLEAN,   // disabled, checked
    ATTR_EXPRESSION,// data-id=variable
    ATTR_CLASS,     // class="container"
    ATTR_ID         // id="main"
} AttributeType;

// ============================================================================
// ESTRUCTURAS PARA ATRIBUTOS
// ============================================================================
typedef struct {
    char* name;
    char* value;
    AttributeType type;
} Attribute;

typedef struct {
    Attribute** attributes;
    int count;
    int capacity;
} AttributeList;

// ============================================================================
// ESTRUCTURA PARA TOKENS
// ============================================================================
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
    int depth;
    AttributeList* attributes;
} Token;

typedef struct {
    Token** tokens;
    int count;
    int capacity;
} TokenList;

// ============================================================================
// ESTRUCTURA PARA NODOS AST
// ============================================================================
typedef struct ASTNode {
    char* tag;
    char* text_content;
    TokenType node_type;
    
    AttributeList* attributes;
    char** classes;
    int class_count;
    char* id;
    
    int depth;
    int line;
    int column;
    
    struct ASTNode* parent;
    struct ASTNode** children;
    int children_count;
    int children_capacity;
    
    int is_void;
    int is_inline;
    int is_block;
} ASTNode;

typedef struct {
    ASTNode** nodes;
    int count;
    int capacity;
} NodeList;

// ============================================================================
// ESTRUCTURA PARA EL CONTEXTO DE PARSING
// ============================================================================
typedef struct {
    TokenList* tokens;
    int current_token;
    int current_depth;
    ASTNode* current_parent;
    ASTNode* root_node;
    
    int use_tabs;
    int tab_size;
    int strict_mode;
} ParserContext;

// ============================================================================
// IMPLEMENTACIÓN INLINE - TokenList
// ============================================================================
static inline TokenList* token_list_create() {
    TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    if (!list) return NULL;
    
    list->capacity = 10;
    list->count = 0;
    list->tokens = (Token**)malloc(sizeof(Token*) * list->capacity);
    
    if (!list->tokens) {
        free(list);
        return NULL;
    }
    
    return list;
}

static inline void token_list_add(TokenList* list, Token* token) {
    if (!list || !token) return;
    
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        Token** new_tokens = (Token**)realloc(list->tokens, sizeof(Token*) * list->capacity);
        if (!new_tokens) return;
        list->tokens = new_tokens;
    }
    
    list->tokens[list->count] = token;
    list->count++;
}

static inline void token_list_free(TokenList* list);

static inline void token_list_free(TokenList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
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

// ============================================================================
// IMPLEMENTACIÓN INLINE - AttributeList
// ============================================================================
static inline AttributeList* attribute_list_create() {
    AttributeList* list = (AttributeList*)malloc(sizeof(AttributeList));
    if (!list) return NULL;
    
    list->capacity = 5;
    list->count = 0;
    list->attributes = (Attribute**)malloc(sizeof(Attribute*) * list->capacity);
    
    if (!list->attributes) {
        free(list);
        return NULL;
    }
    
    return list;
}

static inline void attribute_list_add(AttributeList* list, Attribute* attr) {
    if (!list || !attr) return;
    
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        Attribute** new_attrs = (Attribute**)realloc(list->attributes, sizeof(Attribute*) * list->capacity);
        if (!new_attrs) return;
        list->attributes = new_attrs;
    }
    
    list->attributes[list->count] = attr;
    list->count++;
}

static inline void attribute_list_free(AttributeList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
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

// ============================================================================
// IMPLEMENTACIÓN INLINE - ASTNode
// ============================================================================
static inline ASTNode* ast_node_create(TokenType type, int depth) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->tag = NULL;
    node->text_content = NULL;
    node->node_type = type;
    node->attributes = NULL;
    node->classes = NULL;
    node->class_count = 0;
    node->id = NULL;
    node->depth = depth;
    node->line = 0;
    node->column = 0;
    node->parent = NULL;
    node->children = NULL;
    node->children_count = 0;
    node->children_capacity = 5;
    node->is_void = 0;
    node->is_inline = 0;
    node->is_block = 0;
    
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * node->children_capacity);
    if (!node->children) {
        free(node);
        return NULL;
    }
    
    return node;
}

static inline void ast_node_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    if (parent->children_count >= parent->children_capacity) {
        parent->children_capacity *= 2;
        ASTNode** new_children = (ASTNode**)realloc(parent->children, 
                                                     sizeof(ASTNode*) * parent->children_capacity);
        if (!new_children) return;
        parent->children = new_children;
    }
    
    parent->children[parent->children_count] = child;
    child->parent = parent;
    parent->children_count++;
}

static inline void ast_node_free(ASTNode* node) {
    if (!node) return;
    
    if (node->tag) free(node->tag);
    if (node->text_content) free(node->text_content);
    if (node->id) free(node->id);
    
    if (node->classes) {
        for (int i = 0; i < node->class_count; i++) {
            if (node->classes[i]) free(node->classes[i]);
        }
        free(node->classes);
    }
    
    if (node->attributes) {
        attribute_list_free(node->attributes);
    }
    
    if (node->children) {
        for (int i = 0; i < node->children_count; i++) {
            ast_node_free(node->children[i]);
        }
        free(node->children);
    }
    
    free(node);
}

// ============================================================================
// IMPLEMENTACIÓN INLINE - NodeList
// ============================================================================
static inline NodeList* node_list_create() {
    NodeList* list = (NodeList*)malloc(sizeof(NodeList));
    if (!list) return NULL;
    
    list->capacity = 10;
    list->count = 0;
    list->nodes = (ASTNode**)malloc(sizeof(ASTNode*) * list->capacity);
    
    if (!list->nodes) {
        free(list);
        return NULL;
    }
    
    return list;
}

static inline void node_list_add(NodeList* list, ASTNode* node) {
    if (!list || !node) return;
    
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        ASTNode** new_nodes = (ASTNode**)realloc(list->nodes, sizeof(ASTNode*) * list->capacity);
        if (!new_nodes) return;
        list->nodes = new_nodes;
    }
    
    list->nodes[list->count] = node;
    list->count++;
}

static inline void node_list_free(NodeList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
        ast_node_free(list->nodes[i]);
    }
    
    free(list->nodes);
    free(list);
}

// ============================================================================
// IMPLEMENTACIÓN INLINE - ParserContext
// ============================================================================
static inline ParserContext* parser_context_create() {
    ParserContext* ctx = (ParserContext*)malloc(sizeof(ParserContext));
    if (!ctx) return NULL;
    
    ctx->tokens = NULL;
    ctx->current_token = 0;
    ctx->current_depth = 0;
    ctx->current_parent = NULL;
    ctx->root_node = NULL;
    ctx->use_tabs = 1;
    ctx->tab_size = 4;
    ctx->strict_mode = 0;
    
    return ctx;
}

static inline void parser_context_free(ParserContext* context) {
    if (!context) return;
    
    if (context->tokens) {
        token_list_free(context->tokens);
    }
    
    if (context->root_node) {
        ast_node_free(context->root_node);
    }
    
    free(context);
}

// ============================================================================
// FUNCIONES AUXILIARES PARA DEBUGGING
// ============================================================================
static inline const char* token_type_to_string(TokenType type) {
    switch(type) {
        case TOKEN_TAG: return "TAG";
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_ID: return "ID";
        case TOKEN_ATTRIBUTE: return "ATTRIBUTE";
        case TOKEN_TEXT: return "TEXT";
        case TOKEN_INDENTATION: return "INDENTATION";
        case TOKEN_DOT: return "DOT";
        case TOKEN_PIPE: return "PIPE";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_INTERPOLATION: return "INTERPOLATION";
        case TOKEN_CODE: return "CODE";
        case TOKEN_DOCTYPE: return "DOCTYPE";
        case TOKEN_INCLUDE: return "INCLUDE";
        case TOKEN_EXTENDS: return "EXTENDS";
        case TOKEN_BLOCK: return "BLOCK";
        case TOKEN_MIXIN: return "MIXIN";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

static inline void token_list_print(TokenList* list) {
    if (!list) {
        printf("TokenList es NULL\n");
        return;
    }
    
    printf("=== TokenList (count: %d, capacity: %d) ===\n", list->count, list->capacity);
    for (int i = 0; i < list->count; i++) {
        Token* token = list->tokens[i];
        printf("[%d] Tipo: %-15s | Valor: %-20s | Linea: %d | Columna: %d | Profundidad: %d\n",
               i,
               token_type_to_string(token->type),
               token->value ? token->value : "(null)",
               token->line,
               token->column,
               token->depth);
    }
    printf("=====================================\n\n");
}

static inline void ast_node_print_internal(ASTNode* node, int indent_level);

static inline void ast_node_print_internal(ASTNode* node, int indent_level) {
    if (!node) return;
    
    for (int i = 0; i < indent_level; i++) {
        printf("  ");
    }
    
    printf("<%s", node->tag ? node->tag : "text");
    
    if (node->id) {
        printf(" id=\"%s\"", node->id);
    }
    
    for (int i = 0; i < node->class_count; i++) {
        printf(" class=\"%s\"", node->classes[i]);
    }
    
    printf(">");
    
    if (node->text_content) {
        printf(" %s", node->text_content);
    }
    
    printf(" (depth: %d, children: %d)\n", node->depth, node->children_count);
    
    for (int i = 0; i < node->children_count; i++) {
        ast_node_print_internal(node->children[i], indent_level + 1);
    }
}

static inline void ast_node_print(ASTNode* node, int indent_level) {
    ast_node_print_internal(node, indent_level);
}

#ifdef __cplusplus
}
#endif

#endif // C_PUG_TOKENS_H