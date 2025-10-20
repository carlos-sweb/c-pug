#ifndef PUG_BUG_H
#define PUG_BUG_H

#include <glib.h>
#include <stdio.h>
#include "pug/pug_tokens.h"
#include "pug/pug_ast.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// FUNCIONES AUXILIARES PARA DEPURACIÓN
// ============================================================================

// Convierte un tipo de token a una cadena legible
static inline const char* token_type_to_string(TokenType type) {
    switch (type) {
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
        case TOKEN_BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TOKEN_MIXIN: return "MIXIN";
        case TOKEN_CALL: return "CALL";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

// Imprime una lista de tokens para depuración
static inline void token_list_print(TokenList* list) {
    if (!list) {
        printf("TokenList es NULL\n");
        return;
    }
    
    printf("=== TokenList (count: %d, capacity: %d) ===\n", list->count, list->capacity);
    for (unsigned int i = 0; i < list->count; i++) {
        Token* token = list->tokens[i];
        printf("[%u] Tipo: %-15s | Valor: %-20s | Línea: %u | Columna: %u | Profundidad: %u\n",
               i,
               token_type_to_string(token->type),
               token->value ? token->value : "(null)",
               token->line,
               token->column,
               token->depth);
    }
    printf("=====================================\n\n");
}

// Imprime un nodo AST recursivamente para depuración
static inline void ast_node_print_internal(ASTNode* node, unsigned int indent_level) {
    if (!node) return;
    
    for (unsigned int i = 0; i < indent_level; i++) {
        printf("  ");
    }
    
    printf("<%s", node->tag ? node->tag : "text");
    
    if (node->id) {
        printf(" id=\"%s\"", node->id);
    }
    
    if (node->class_count > 0) {
        printf(" class=\"");
        for (unsigned int i = 0; i < node->class_count; i++) {
            if (i > 0) printf(" ");
            printf("%s", node->classes[i]);
        }
        printf("\"");
    }
    
    printf(">");
    
    if (node->text_content) {
        printf(" %s", node->text_content);
    }
    
    printf(" (type: %s, depth: %u, children: %u, line: %u, col: %u)\n",
           token_type_to_string(node->node_type), node->depth, node->children_count,
           node->line, node->column);
    
    for (unsigned int i = 0; i < node->children_count; i++) {
        ast_node_print_internal(node->children[i], indent_level + 1);
    }
}

// Imprime un nodo AST para depuración
static inline void ast_node_print(ASTNode* node, unsigned int indent_level) {
    ast_node_print_internal(node, indent_level);
}

#ifdef __cplusplus
}
#endif

#endif // PUG_BUG_H