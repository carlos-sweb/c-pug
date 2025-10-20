#ifndef PUG_RENDERER_H
#define PUG_RENDERER_H

#include "pug_tokens.h"
#include "pug_ast.h"
#include "pug_attribute_list.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Lista de tags HTML void (no requieren cierre)
static const char* void_tags[] = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr", NULL
};

// Verifica si un tag es void
static inline int is_void_tag(const char* tag) {
    if (!tag) return 0;
    for (int i = 0; void_tags[i]; i++) {
        if (g_strcmp0(tag, void_tags[i]) == 0) return 1;
    }
    return 0;
}

// Escapa caracteres especiales para HTML
static inline char* escape_html(const char* input) {
    if (!input) return g_strdup("");
    GString* escaped = g_string_new("");
    if (!escaped) return NULL;
    for (const char* p = input; *p; p++) {
        switch (*p) {
            case '&': g_string_append(escaped, "&amp;"); break;
            case '<': g_string_append(escaped, "&lt;"); break;
            case '>': g_string_append(escaped, "&gt;"); break;
            case '"': g_string_append(escaped, "&quot;"); break;
            case '\'': g_string_append(escaped, "&#39;"); break;
            default: g_string_append_c(escaped, *p); break;
        }
    }
    char* result = g_string_free(escaped, FALSE);
    return result;
}

// Genera la indentación para una línea
static inline void append_indent(GString* output, unsigned int depth, unsigned int use_tabs, unsigned int tab_size) {
    if (!output) return;
    for (unsigned int i = 0; i < depth; i++) {
        if (use_tabs) {
            g_string_append_c(output, '\t');
        } else {
            for (unsigned int j = 0; j < tab_size; j++) {
                g_string_append_c(output, ' ');
            }
        }
    }
}

// Renderiza los atributos de un nodo
static inline void render_attributes(GString* output, AttributeList* attrs) {
    if (!output || !attrs) return;
    for (unsigned int i = 0; i < attrs->count; i++) {
        Attribute* attr = attrs->attributes[i];
        if (!attr || !attr->name) continue;
        g_string_append_c(output, ' ');
        g_string_append(output, attr->name);
        if (attr->type != ATTR_BOOLEAN && attr->value) {
            g_string_append_printf(output, "=\"%s\"", attr->value);
        }
    }
}

// Renderiza las clases de un nodo
static inline void render_classes(GString* output, char** classes, unsigned int class_count) {
    if (!output || class_count == 0) return;
    g_string_append(output, " class=\"");
    for (unsigned int i = 0; i < class_count; i++) {
        if (i > 0) g_string_append_c(output, ' ');
        char* escaped_class = escape_html(classes[i]);
        g_string_append(output, escaped_class ? escaped_class : "");
        g_free(escaped_class);
    }
    g_string_append_c(output, '"');
}

// Renderiza un nodo AST recursivamente
static inline void render_node(GString* output, ASTNode* node, unsigned int use_tabs, unsigned int tab_size, int minify) {
    if (!node || !output) return;

    // Ignorar nodos raíz artificiales
    if (node->tag && g_strcmp0(node->tag, "root") == 0) {
        for (unsigned int i = 0; i < node->children_count; i++) {
            render_node(output, node->children[i], use_tabs, tab_size, minify);
        }
        return;
    }

    // Añadir indentación solo si NO está minificado
    if (!minify) {
        append_indent(output, node->depth, use_tabs, tab_size);
    }

    // Manejar nodos según su tipo
    switch (node->node_type) {
        case TOKEN_DOCTYPE:
            g_string_append_printf(output, "<!DOCTYPE %s>", node->text_content ? node->text_content : "html");
            if (!minify) g_string_append_c(output, '\n');
            break;

        case TOKEN_TEXT:
        case TOKEN_PIPE:
        case TOKEN_DOT:
            if (node->text_content) {
                char* escaped_text = escape_html(node->text_content);
                g_string_append(output, escaped_text);
                g_free(escaped_text);
            }
            if (!minify) g_string_append_c(output, '\n');
            break;

        case TOKEN_INTERPOLATION:
            if (node->text_content) {
                g_string_append_printf(output, "#{ %s }", node->text_content);
            }
            if (!minify) g_string_append_c(output, '\n');
            break;

        case TOKEN_COMMENT:
            // Los comentarios solo se renderizan si NO está minificado
            if (minify == 0) {
                g_string_append(output, "<!-- ");
                if (node->text_content) {
                    char* escaped_comment = escape_html(node->text_content);
                    g_string_append(output, escaped_comment);
                    g_free(escaped_comment);
                }
                g_string_append(output, " -->\n");
            }
            break;

        case TOKEN_BLOCK_COMMENT:
            // Los block comments no se renderizan nunca
            break;

        case TOKEN_CODE:
        case TOKEN_INCLUDE:
        case TOKEN_EXTENDS:
        case TOKEN_MIXIN:
        case TOKEN_CALL:
            // Renderizar como comentario para debugging solo si NO está minificado
            if (minify == 0) {
                g_string_append_printf(output, "<!-- %s: %s -->\n",
                                      token_type_to_string(node->node_type),
                                      node->text_content ? node->text_content : "");
            }
            break;

        case TOKEN_TAG:
            // Abrir tag
            g_string_append_printf(output, "<%s", node->tag ? node->tag : "div");

            // Renderizar ID
            if (node->id) {
                char* escaped_id = escape_html(node->id);
                g_string_append_printf(output, " id=\"%s\"", escaped_id);
                g_free(escaped_id);
            }

            // Renderizar clases
            render_classes(output, node->classes, node->class_count);

            // Renderizar atributos
            render_attributes(output, node->attributes);

            // Cerrar tag de apertura
            if (node->is_void || is_void_tag(node->tag)) {
                g_string_append(output, " />");
                if (!minify) g_string_append_c(output, '\n');
                return; // Tags void no tienen hijos ni cierre
            } else {
                g_string_append_c(output, '>');
            }

            // Renderizar contenido de texto inline
            if (node->text_content && node->is_inline) {
                char* escaped_text = escape_html(node->text_content);
                g_string_append(output, escaped_text);
                g_free(escaped_text);
            } else if (node->text_content) {
                // Texto en bloque
                if (!minify) {
                    g_string_append_c(output, '\n');
                    append_indent(output, node->depth + 1, use_tabs, tab_size);
                }
                char* escaped_text = escape_html(node->text_content);
                g_string_append(output, escaped_text);
                g_free(escaped_text);
            }

            // Renderizar hijos
            if (node->children_count > 0) {
                if (!minify && (!node->text_content || node->is_block)) {
                    g_string_append_c(output, '\n');
                }
                for (unsigned int i = 0; i < node->children_count; i++) {
                    render_node(output, node->children[i], use_tabs, tab_size, minify);
                }
                // Añadir indentación para tag de cierre
                if (!minify && !node->is_inline) {
                    append_indent(output, node->depth, use_tabs, tab_size);
                }
            } else if (!minify && node->text_content && node->is_block) {
                g_string_append_c(output, '\n');
                append_indent(output, node->depth, use_tabs, tab_size);
            }

            // Cerrar tag
            g_string_append_printf(output, "</%s>", node->tag ? node->tag : "div");
            if (!minify) g_string_append_c(output, '\n');
            break;

        default:
            // Ignorar nodos desconocidos
            break;
    }
}

// Función principal de renderizado
static inline char* render_ast(ASTNode* root, unsigned int use_tabs, unsigned int tab_size, unsigned int minify) {
    if (!root) return NULL;

    GString* output = g_string_new("");
    if (!output) return NULL;

    render_node(output, root, use_tabs, tab_size , minify );

    char* result = g_string_free(output, FALSE);
    return result;
}



#ifdef __cplusplus
}
#endif

#endif // PUG_RENDERER_H



