#ifndef C_PUG_TOKENS_H
#define C_PUG_TOKENS_H

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
    int depth;          // Nivel de indentación
    AttributeList* attributes; // Solo para TOKEN_TAG con atributos
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
    // Información básica del nodo
    char* tag;              // Nombre del tag (NULL para nodos de texto)
    char* text_content;     // Contenido de texto (para nodos de texto)
    TokenType node_type;    // Tipo de nodo
    
    // Atributos
    AttributeList* attributes;
    char** classes;         // Lista de clases CSS
    int class_count;
    char* id;               // ID del elemento
    
    // Jerarquía y posición
    int depth;              // Nivel de indentación (0 = raíz)
    int line;               // Línea en el archivo original
    int column;             // Columna en el archivo original
    
    // Relaciones familiares
    struct ASTNode* parent;
    struct ASTNode** children;
    int children_count;
    int children_capacity;
    
    // Información específica de Pug
    int is_void;            // Si es elemento auto-cerrado
    int is_inline;          // Si el texto es inline
    int is_block;           // Si es un bloque de contenido
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
    
    // Configuración
    int use_tabs;           // 1 para tabs, 0 para espacios
    int tab_size;           // Si use_tabs=0, tamaño del tab en espacios
    int strict_mode;        // Modo estricto de parsing
} ParserContext;

// ============================================================================
// FUNCIONES DE CREACIÓN Y DESTRUCCIÓN
// ============================================================================

// Funciones para TokenList

TokenList* token_list_create() {
    TokenList* list = malloc(sizeof(TokenList));
    list->count = 0;
    list->capacity = 10;
    list->tokens = malloc(sizeof(Token*) * list->capacity);
    return list;
}

void token_list_add(TokenList* list, Token* token) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, sizeof(Token*) * list->capacity);
    }
    list->tokens[list->count++] = token;
}

void token_list_free(TokenList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->tokens[i]->value);
        if (list->tokens[i]->attributes) {
            attribute_list_free(list->tokens[i]->attributes);
        }
        free(list->tokens[i]);
    }
    free(list->tokens);
    free(list);
}

// Funciones para AttributeList
AttributeList* attribute_list_create() {
    AttributeList* list = malloc(sizeof(AttributeList));
    list->count = 0;
    list->capacity = 5;
    list->attributes = malloc(sizeof(Attribute*) * list->capacity);
    return list;
}

void attribute_list_add(AttributeList* list, Attribute* attr) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->attributes = realloc(list->attributes, sizeof(Attribute*) * list->capacity);
    }
    list->attributes[list->count++] = attr;
}


void attribute_list_free(AttributeList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->attributes[i]->name);
        free(list->attributes[i]->value);
        free(list->attributes[i]);
    }
    free(list->attributes);
    free(list);
}

// Funciones para ASTNode
ASTNode* ast_node_create(TokenType type, int depth) {
    ASTNode* node = malloc(sizeof(ASTNode));
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
    node->children_capacity = 0;
    node->is_void = 0;
    node->is_inline = 0;
    node->is_block = 0;
    return node;
}


void ast_node_add_child(ASTNode* parent, ASTNode* child) {
    if (parent->children_count >= parent->children_capacity) {
        parent->children_capacity = parent->children_capacity == 0 ? 5 : parent->children_capacity * 2;
        parent->children = realloc(parent->children, sizeof(ASTNode*) * parent->children_capacity);
    }
    parent->children[parent->children_count++] = child;
    child->parent = parent;
}

void ast_node_free(ASTNode* node) {
    if (!node) return;
    
    free(node->tag);
    free(node->text_content);
    free(node->id);
    
    if (node->attributes) {
        attribute_list_free(node->attributes);
    }
    
    if (node->classes) {
        for (int i = 0; i < node->class_count; i++) {
            free(node->classes[i]);
        }
        free(node->classes);
    }
    
    for (int i = 0; i < node->children_count; i++) {
        ast_node_free(node->children[i]);
    }
    
    free(node->children);
    free(node);
}

// Funciones para NodeList
NodeList* node_list_create() {
    NodeList* list = malloc(sizeof(NodeList));
    list->count = 0;
    list->capacity = 10;
    list->nodes = malloc(sizeof(ASTNode*) * list->capacity);
    return list;
}

void node_list_add(NodeList* list, ASTNode* node) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->nodes = realloc(list->nodes, sizeof(ASTNode*) * list->capacity);
    }
    list->nodes[list->count++] = node;
}

void node_list_free(NodeList* list) {
    for (int i = 0; i < list->count; i++) {
        ast_node_free(list->nodes[i]);
    }
    free(list->nodes);
    free(list);
}

// Función para ParserContext
ParserContext* parser_context_create() {
    ParserContext* context = malloc(sizeof(ParserContext));
    context->tokens = token_list_create();
    context->current_token = 0;
    context->current_depth = 0;
    context->current_parent = NULL;
    context->root_node = ast_node_create(TOKEN_TAG, 0);
    context->root_node->tag = strdup("root");
    context->use_tabs = 1; // Por defecto usar tabs
    context->tab_size = 4;
    context->strict_mode = 0;
    return context;
}

void parser_context_free(ParserContext* context) {
    if (!context) return;
    
    if (context->tokens) {
        token_list_free(context->tokens);
    }
    
    if (context->root_node) {
        ast_node_free(context->root_node);
    }
    
    free(context);
}

#ifdef __cplusplus
}
#endif

#endif // C_PUG_TOKENS_H