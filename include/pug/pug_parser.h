#ifndef PUG_PARSER_H
#define PUG_PARSER_H

#include "pug/pug_tokens.h"
#include "pug/pug_ast.h"
#include "pug/pug_attribute_list.h"
#include "pug/pug_token_list.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Crea un contexto de parsing
static inline ParserContext* parser_context_create(void) {
    ParserContext* ctx = (ParserContext*)malloc(sizeof(ParserContext));
    if (!ctx) return NULL;
    ctx->tokens = NULL;
    ctx->root_node = NULL;
    ctx->current_parent = NULL;
    ctx->current_token = 0;
    ctx->current_depth = 0;
    ctx->use_tabs = 0;
    ctx->tab_size = 2;
    return ctx;
}

// Libera un contexto de parsing y sus recursos
static inline void parser_context_free(ParserContext* ctx) {
    if (!ctx) return;
    if (ctx->tokens) token_list_free(ctx->tokens);
    if (ctx->root_node) ast_node_free(ctx->root_node);
    free(ctx);
}

// Obtiene el token actual
static inline Token* parser_get_current_token(ParserContext* ctx) {
    if (!ctx || !ctx->tokens || ctx->current_token >= ctx->tokens->count) return NULL;
    return ctx->tokens->tokens[ctx->current_token];
}

// Avanza al siguiente token
static inline void parser_advance_token(ParserContext* ctx) {
    if (ctx) ctx->current_token++;
}

// Parsea atributos de un token ATTRIBUTE y los añade al nodo
// - Divide el valor del token por comas (e.g., lang='es',charset='UTF-8')
// - Para cada par, divide por igual (e.g., lang='es' -> ["lang", "'es'"])
// - Limpia comillas simples o dobles del valor
// - Ignora atributos con nombre o valor vacíos, o malformados
static inline void parse_attributes(ParserContext* ctx, ASTNode* node) {
    Token* token = parser_get_current_token(ctx);
    if (!token || token->type != TOKEN_ATTRIBUTE || !token->value) return;
    
    if (!node->attributes) {
        node->attributes = attribute_list_create();
        if (!node->attributes) return;
    }
    
    gchar** attr_pairs = g_strsplit(token->value, ",", -1);
    for (gchar** pair = attr_pairs; *pair; pair++) {
        gchar* attr_str = g_strstrip(*pair);
        if (!*attr_str) continue; // Ignorar pares vacíos
        
        gchar** kv = g_strsplit(attr_str, "=", 2);
        if (kv[0] && kv[1] && *kv[0]) { // Verificar que el nombre no esté vacío
            Attribute* attr = (Attribute*)malloc(sizeof(Attribute));
            if (!attr) {
                g_strfreev(kv);
                continue;
            }
            // Inicializar explícitamente
            attr->name = NULL;
            attr->value = NULL;
            attr->type = ATTR_STRING;
            
            attr->name = g_strstrip(g_strdup(kv[0]));
            if (!attr->name || !*attr->name) {
                free(attr);
                g_strfreev(kv);
                continue; // Ignorar nombres vacíos
            }
            
            // Limpiar espacios y comillas del valor
            gchar* value = g_strstrip(kv[1]);
            if (*value) { // Verificar que el valor no esté vacío
                if ((value[0] == '"' && value[strlen(value) - 1] == '"') ||
                    (value[0] == '\'' && value[strlen(value) - 1] == '\'')) {
                    value[strlen(value) - 1] = '\0';
                    attr->value = g_strdup(value + 1);
                } else {
                    attr->value = g_strdup(value);
                }
            } else {
                attr->value = g_strdup(""); // Valor vacío permitido
            }
            
            attribute_list_add(node->attributes, attr);
        }
        g_strfreev(kv);
    }
    g_strfreev(attr_pairs);
    parser_advance_token(ctx);
}

// Añade una clase al nodo
static inline void parse_classes(ASTNode* node, const char* class_name) {
    if (!node || !class_name || !*class_name) return;
    if (!node->classes) {
        node->classes = (char**)malloc(sizeof(char*) * 5);
        node->class_count = 0;
    } else if (node->class_count % 5 == 0) {
        char** new_classes = (char**)realloc(node->classes, sizeof(char*) * (node->class_count + 5));
        if (!new_classes) return;
        node->classes = new_classes;
    }
    node->classes[node->class_count++] = g_strdup(class_name);
}

// Parsea un nodo individual basado en el token actual
static inline ASTNode* parse_node(ParserContext* ctx) {
    Token* token = parser_get_current_token(ctx);
    if (!token || token->type == TOKEN_EOF) return NULL;
    
    ASTNode* node = ast_node_create(token->type, token->depth);
    if (!node) return NULL;
    node->line = token->line;
    node->column = token->column;
    
    #ifdef DEBUG
    g_print("Parsing node: type=%s, value=%s, depth=%d, line=%d, col=%d\n",
            token_type_to_string(token->type), token->value ? token->value : "", token->depth, token->line, token->column);
    #endif
    
    switch (token->type) {
        case TOKEN_TAG:
            node->tag = g_strdup(token->value ? token->value : "div");
            parser_advance_token(ctx);
            break;
        case TOKEN_TEXT:
        case TOKEN_PIPE:
        case TOKEN_DOT:
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            node->node_type = TOKEN_TEXT;
            node->is_block = (token->type == TOKEN_DOT || token->type == TOKEN_PIPE);
            parser_advance_token(ctx);
            return node;
        case TOKEN_CODE:
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_COMMENT:
        case TOKEN_BLOCK_COMMENT:
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_INTERPOLATION:
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_DOCTYPE:
            node->tag = g_strdup("doctype");
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : "html"));
            parser_advance_token(ctx);
            return node;
        case TOKEN_INCLUDE:
            node->tag = g_strdup("include");
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_EXTENDS:
            node->tag = g_strdup("extends");
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_MIXIN:
            node->tag = g_strdup("mixin");
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        case TOKEN_CALL:
            node->tag = g_strdup("call");
            node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
            return node;
        default:
            ast_node_free(node);
            parser_advance_token(ctx);
            return NULL;
    }
    
    while (1) {
        token = parser_get_current_token(ctx);
        if (!token || token->depth != node->depth) break;
        
        if (token->type == TOKEN_ID) {
            node->id = g_strstrip(g_strdup(token->value ? token->value : ""));
            parser_advance_token(ctx);
        } else if (token->type == TOKEN_CLASS) {
            parse_classes(node, token->value ? token->value : "");
            parser_advance_token(ctx);
        } else if (token->type == TOKEN_ATTRIBUTE) {
            parse_attributes(ctx, node);
        } else if (token->type == TOKEN_TEXT || token->type == TOKEN_INTERPOLATION) {
            if (node->text_content) {
                char* new_text = g_strconcat(node->text_content, " ", token->value, NULL);
                g_free(node->text_content);
                node->text_content = g_strstrip(g_strdup(new_text));
                g_free(new_text);
            } else {
                node->text_content = g_strstrip(g_strdup(token->value ? token->value : ""));
            }
            node->is_inline = 1;
            parser_advance_token(ctx);
        } else {
            break;
        }
    }
    
    return node;
}

// Parsea una lista de tokens y construye el AST
static inline ASTNode* parse_pug(ParserContext* ctx) {
    if (!ctx || !ctx->tokens) return NULL;
    
    ctx->root_node = ast_node_create(TOKEN_TAG, -1);
    if (!ctx->root_node) return NULL;
    ctx->root_node->tag = g_strdup("root");
    ctx->current_parent = ctx->root_node;
    ctx->current_depth = -1;
    
    while (1) {
        ASTNode* node = parse_node(ctx);
        if (!node) {
            if (parser_get_current_token(ctx) && parser_get_current_token(ctx)->type == TOKEN_EOF) break;
            parser_advance_token(ctx);
            continue;
        }
        
        #ifdef DEBUG
        g_print("Adding node: tag=%s, depth=%d, parent=%s\n",
                node->tag ? node->tag : (node->text_content ? node->text_content : "unknown"),
                node->depth,
                ctx->current_parent->tag ? ctx->current_parent->tag : "root");
        #endif
        
        // Retroceder al padre correcto basándose en la profundidad del nodo
        while (ctx->current_parent->parent && node->depth <= ctx->current_parent->depth) {
            ctx->current_parent = ctx->current_parent->parent;
            #ifdef DEBUG
            g_print("Backtracked to parent: %s, depth=%d\n",
                    ctx->current_parent->tag ? ctx->current_parent->tag : "root",
                    ctx->current_parent->depth);
            #endif
        }
        
        // Añadir el nodo como hijo del padre actual
        ast_node_add_child(ctx->current_parent, node);
        ctx->current_depth = node->depth;
        
        #ifdef DEBUG
        g_print("Added node '%s' (depth=%d) as child of '%s' (depth=%d)\n",
                node->tag ? node->tag : (node->text_content ? node->text_content : "unknown"),
                node->depth,
                ctx->current_parent->tag ? ctx->current_parent->tag : "root",
                ctx->current_parent->depth);
        #endif
        
        // Si el próximo token tiene mayor profundidad, este nodo será el nuevo padre
        Token* next_token = (ctx->current_token < ctx->tokens->count) ? 
                           ctx->tokens->tokens[ctx->current_token] : NULL;
        if (next_token && next_token->depth > node->depth) {
            ctx->current_parent = node;
            #ifdef DEBUG
            g_print("Set current_parent to: %s\n", node->tag ? node->tag : "unknown");
            #endif
        }
    }
    
    return ctx->root_node;
}

#ifdef __cplusplus
}
#endif

#endif // PUG_PARSER_H