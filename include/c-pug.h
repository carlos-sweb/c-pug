#ifndef C_PUG_H
#define C_PUG_H

#include <glib.h>
#include "pug/pug_tokens.h"
#include "pug/pug_token_list.h"
#include "pug/pug_attribute_list.h"
#include "pug/pug_ast.h"
#include "pug/pug_parser.h"
#include "pug/pug_tokenizer.h"
#include "pug/pug_renderer.h"
#include "pug/pug_bug.h"

#ifdef __cplusplus
extern "C" {
#endif

// Punto de entrada principal para procesar un archivo Pug
static inline char* process_pug_file(
    const char* content, unsigned int use_tabs, unsigned int tab_size , unsigned int minify) {
    // Tokenizar
    TokenList* tokens = tokenize_file(content);
    if (!tokens) {
        g_print("Error: Fallo en tokenización\n");
        return NULL;
    }

    // Crear contexto de parsing
    ParserContext* ctx = parser_context_create();
    if (!ctx) {
        token_list_free(tokens);
        g_print("Error: Fallo en creación de ParserContext\n");
        return NULL;
    }
    ctx->tokens = tokens;
    ctx->use_tabs = use_tabs;
    ctx->tab_size = tab_size;

    // Parsear
    ASTNode* root = parse_pug(ctx);
    if (!root) {
        parser_context_free(ctx);
        g_print("Error: Fallo en parsing\n");
        return NULL;
    }

    // Opcional: Imprimir AST para depuración
    #ifdef DEBUG
    ast_node_print(root, 0);
    #endif

    // Renderizar
    char* result = render_ast(root, ctx->use_tabs, ctx->tab_size,minify);
    parser_context_free(ctx); // Libera tokens y root
    if (!result) {
        g_print("Error: Fallo en renderizado\n");
    }

    return result;
}

// Libera la memoria del resultado renderizado
static inline void render_free(char* rendered) {
    if (rendered) g_free(rendered);
}

#ifdef __cplusplus
}
#endif

#endif // C_PUG_H