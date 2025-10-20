#ifndef PUG_TOKENIZER_H
#define PUG_TOKENIZER_H

#include <glib.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// FUNCIONES DE UTILIDAD PARA INDENTACIÓN
// ============================================================================

// Cuenta el nivel de indentación (tabs o espacios)
static inline int count_indent_level(const gchar *line) {
    unsigned int level = 0;
    unsigned int i = 0;
    
    // Contar cada tab O cada N espacios como 1 nivel
    while (line[i] == '\t' || line[i] == ' ') {
        if (line[i] == '\t') {
            level++;      // ← Cada tab = 1 nivel
            i++;
        } else if (line[i] == ' ') {
            unsigned int space_count = 0;
            while (line[i] == ' ') {
                space_count++;
                i++;
            }
            level += space_count / 2;  // ← Cada 2 espacios = 1 nivel
            // Removed break to handle mixed indentation (spaces followed by tabs)
        }
    }
    
    return level;
}

// Salta espacios y tabs al inicio
static inline unsigned int skip_whitespace(const gchar *line, unsigned int pos) {
    while (line[pos] == ' ' || line[pos] == '\t') {
        pos++;
    }
    return pos;
}

// ============================================================================
// FUNCIÓN PARA CREAR TOKENS
// ============================================================================


static inline Token* create_token(TokenType type, const gchar *value, unsigned int depth, unsigned int line, unsigned int column) {
    //Token *token = (Token*)malloc(sizeof(Token));
    Token *token = g_new(Token, 1);

    if (!token) return NULL;    
    token->type = type;
    token->value = value ? g_strdup(value) : NULL;
    token->line = line;
    token->column = column;
    token->depth = depth;
    token->attributes = NULL;
    return token;
}

// ============================================================================
// TOKENIZADOR DE UNA LÍNEA
// ============================================================================

static inline void tokenize_line(TokenList *token_list, const gchar *line, unsigned int line_number) {
    // Contar indentación
    unsigned int indent_level = count_indent_level(line);
    unsigned int pos = skip_whitespace(line, 0);
    unsigned int column = pos;
    
    // Línea vacía o solo espacios
    if (line[pos] == '\0') {
        return;
    }
    
    // Detectar DOCTYPE
    if (g_str_has_prefix(&line[pos], "doctype")) {
        pos += 7; // longitud de "doctype"
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        if(!buffer) return;
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_DOCTYPE, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar INCLUDE
    if (g_str_has_prefix(&line[pos], "include")) {
        pos += 7; // longitud de "include"
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        if(!buffer) return;
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_INCLUDE, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar EXTENDS
    if (g_str_has_prefix(&line[pos], "extends")) {
        pos += 7; // longitud de "extends"
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        if(!buffer) return;
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_EXTENDS, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar MIXIN
    if (g_str_has_prefix(&line[pos], "mixin")) {
        pos += 5; // longitud de "mixin"
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        if(!buffer) return;
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_MIXIN, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar CALL (para mixins, inicia con +)
    if (line[pos] == '+') {
        pos++;
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        if(!buffer) return;
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_CALL, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar COMENTARIOS (//- para block, // para normal)
    if (line[pos] == '/' && line[pos + 1] == '/') {
        if (line[pos + 2] == '-') {
            pos += 3;
            Token *token = create_token(TOKEN_BLOCK_COMMENT, &line[pos], indent_level, line_number, column);
            token_list_add(token_list, token);
            return;
        } else {
            pos += 2;
            Token *token = create_token(TOKEN_COMMENT, &line[pos], indent_level, line_number, column);
            token_list_add(token_list, token);
            return;
        }
    }
    
    // Detectar PIPE (|)
    if (line[pos] == '|') {
        pos++;
        pos = skip_whitespace(line, pos);
        Token *token = create_token(TOKEN_PIPE, &line[pos], indent_level, line_number, column);
        token_list_add(token_list, token);
        return;
    }
    
    // Detectar CÓDIGO (-)
    if (line[pos] == '-') {
        pos++;
        pos = skip_whitespace(line, pos);
        Token *token = create_token(TOKEN_CODE, &line[pos], indent_level, line_number, column);
        token_list_add(token_list, token);
        return;
    }
    
    // Procesar resto de la línea (tags, clases, ids, atributos, texto con interpolaciones)
    GString *buffer = g_string_new("");
    
    while (line[pos] != '\0') {
        gchar c = line[pos];
        
        // Detectar ID (#)
        if (c == '#') {
            pos++;
            column = pos;
            g_string_truncate(buffer, 0);
            
            while (isalnum(line[pos]) || line[pos] == '-' || line[pos] == '_') {
                g_string_append_c(buffer, line[pos]);
                pos++;
            }
            
            Token *token = create_token(TOKEN_ID, buffer->str, indent_level, line_number, column);
            token_list_add(token_list, token);
        }
        // Detectar CLASS o DOT (.)
        else if (c == '.') {
            pos++;
            column = pos;
            g_string_truncate(buffer, 0);
            
            while (isalnum(line[pos]) || line[pos] == '-' || line[pos] == '_') {
                g_string_append_c(buffer, line[pos]);
                pos++;
            }
            
            TokenType type = (buffer->len > 0) ? TOKEN_CLASS : TOKEN_DOT;
            Token *token = create_token(type, buffer->str, indent_level, line_number, column);
            token_list_add(token_list, token);
        }
        // Detectar ATRIBUTOS (paréntesis)
        else if (c == '(') {
            pos++;
            g_string_truncate(buffer, 0);
            
            int paren_level = 1;
            while (line[pos] != '\0' && paren_level > 0) {
                if (line[pos] == '(') paren_level++;
                if (line[pos] == ')') paren_level--;
                
                if (paren_level > 0) {
                    g_string_append_c(buffer, line[pos]);
                }
                pos++;
            }
            
            Token *token = create_token(TOKEN_ATTRIBUTE, buffer->str, indent_level, line_number, column);
            token_list_add(token_list, token);
        }
        // Detectar TAG (letra al inicio o después de espacio)
        else if (isalpha(c)) {
            column = pos;
            g_string_truncate(buffer, 0);
            
            while (isalnum(line[pos]) || line[pos] == '-') {
                g_string_append_c(buffer, line[pos]);
                pos++;
            }
            
            Token *token = create_token(TOKEN_TAG, buffer->str, indent_level, line_number, column);
            token_list_add(token_list, token);
        }
        // Detectar TEXTO (después de espacio), con manejo de interpolaciones #{}
        else if (c == ' ') {
            pos = skip_whitespace(line, pos);
            
            // Si hay contenido después del espacio y no es un símbolo especial
            if (line[pos] != '\0' && line[pos] != '#' && line[pos] != '.') {
                column = pos;
                
                // Procesar texto con posible interpolación
                while (line[pos] != '\0') {
                    g_string_truncate(buffer, 0);
                    
                    // Coleccionar texto hasta # o fin
                    while (line[pos] != '\0' && line[pos] != '#') {
                        g_string_append_c(buffer, line[pos]);
                        pos++;
                    }
                    
                    // Agregar TOKEN_TEXT si hay contenido
                    if (buffer->len > 0) {
                        Token *token = create_token(TOKEN_TEXT, buffer->str, indent_level, line_number, column);
                        token_list_add(token_list, token);
                    }
                    
                    // Si es interpolación #{}
                    if (line[pos] == '#' && line[pos + 1] == '{') {
                        pos += 2;  // Saltar #{
                        column = pos;
                        g_string_truncate(buffer, 0);
                        
                        // Coleccionar hasta }
                        while (line[pos] != '\0' && line[pos] != '}') {
                            g_string_append_c(buffer, line[pos]);
                            pos++;
                        }
                        
                        if (line[pos] == '}') {
                            pos++;  // Saltar }
                        }
                        
                        Token *token = create_token(TOKEN_INTERPOLATION, buffer->str, indent_level, line_number, column);
                        token_list_add(token_list, token);
                    } else if (line[pos] == '#') {
                        // # solo, tratar como texto
                        g_string_append_c(buffer, '#');
                        pos++;
                    }
                }
                break; // El texto consume el resto de la línea
            }
        }
        else {
            pos++;
        }
    }
    
    g_string_free(buffer, TRUE);
}

// ============================================================================
// FUNCIÓN PRINCIPAL DE TOKENIZACIÓN
// ============================================================================

TokenList* tokenize_file(const gchar *content) {
    TokenList *token_list = token_list_create();
    if (!token_list) {
        g_print("Error: No se pudo crear TokenList\n");
        return NULL;
    }
    
    gchar **lines = g_strsplit(content, "\n", 0);
    
    for (int i = 0; i < g_strv_length(lines); i++) {
        tokenize_line(token_list, lines[i], i + 1);
    }
    
    // Agregar token EOF al final
    Token *eof = create_token(TOKEN_EOF, NULL, 0, g_strv_length(lines) + 1, 0);
    token_list_add(token_list, eof);
    
    g_strfreev(lines);
    return token_list;
}

#ifdef __cplusplus
}
#endif

#endif // PUG_TOKENIZER_H