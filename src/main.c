#include <glib-2.0/glib.h>
#include <stdio.h>
#include <ctype.h>
#include <c-pug-tokens.h>

// ============================================================================
// FUNCIONES DE UTILIDAD PARA INDENTACIÓN
// ============================================================================

// Cuenta el nivel de indentación (tabs o espacios)
static inline int count_indent_level(const gchar *line) {
    int level = 0;
    int i = 0;
    
    // Contar cada tab O cada N espacios como 1 nivel
    while (line[i] == '\t' || line[i] == ' ') {
        if (line[i] == '\t') {
            level++;      // ← Cada tab = 1 nivel
            i++;
        } else if (line[i] == ' ') {
            int space_count = 0;
            while (line[i] == ' ') {
                space_count++;
                i++;
            }
            level += space_count / 2;  // ← Cada 2 espacios = 1 nivel
            break;
        }
    }
    
    return level;
}

// Salta espacios y tabs al inicio
static inline int skip_whitespace(const gchar *line, int pos) {
    while (line[pos] == ' ' || line[pos] == '\t') {
        pos++;
    }
    return pos;
}

// ============================================================================
// FUNCIÓN PARA CREAR TOKENS
// ============================================================================

static inline Token* create_token(TokenType type, const gchar *value, int depth, int line, int column) {
    Token *token = (Token*)malloc(sizeof(Token));
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

static inline void tokenize_line(TokenList *token_list, const gchar *line, int line_number) {
    // Contar indentación
    int indent_level = count_indent_level(line);
    int pos = skip_whitespace(line, 0);
    int column = pos;
    
    // Línea vacía o solo espacios
    if (line[pos] == '\0') {
        return;
    }
    
    // Detectar DOCTYPE
    if (g_str_has_prefix(&line[pos], "doctype")) {
        pos += 7; // longitud de "doctype"
        pos = skip_whitespace(line, pos);
        
        GString *buffer = g_string_new("");
        while (line[pos] != '\0') {
            g_string_append_c(buffer, line[pos]);
            pos++;
        }
        
        Token *token = create_token(TOKEN_DOCTYPE, buffer->str, indent_level, line_number, column);
        token_list_add(token_list, token);
        g_string_free(buffer, TRUE);
        return;
    }
    
    // Detectar COMENTARIOS (//)
    if (line[pos] == '/' && line[pos + 1] == '/') {
        pos += 2;
        Token *token = create_token(TOKEN_COMMENT, &line[pos], indent_level, line_number, column);
        token_list_add(token_list, token);
        return;
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
    
    // Procesar resto de la línea (tags, clases, ids, texto)
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
        // Detectar CLASS (.)
        else if (c == '.') {
            pos++;
            column = pos;
            g_string_truncate(buffer, 0);
            
            while (isalnum(line[pos]) || line[pos] == '-' || line[pos] == '_') {
                g_string_append_c(buffer, line[pos]);
                pos++;
            }
            
            Token *token = create_token(TOKEN_CLASS, buffer->str, indent_level, line_number, column);
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
        // Detectar TEXTO (después de espacio)
        else if (c == ' ') {
            pos = skip_whitespace(line, pos);
            
            // Si hay contenido después del espacio y no es un símbolo especial
            if (line[pos] != '\0' && line[pos] != '#' && line[pos] != '.') {
                column = pos;
                g_string_truncate(buffer, 0);
                
                while (line[pos] != '\0') {
                    g_string_append_c(buffer, line[pos]);
                    pos++;
                }
                
                // Solo agregar si el buffer no está vacío
                if (buffer->len > 0) {
                    Token *token = create_token(TOKEN_TEXT, buffer->str, indent_level, line_number, column);
                    token_list_add(token_list, token);
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

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char *argv[]) {
    if (argc != 2) {
        g_print("Uso: %s <archivo.pug>\n", argv[0]);
        return 1;
    }
    
    gchar *content = NULL;
    GError *error = NULL;
    
    if (g_file_get_contents(argv[1], &content, NULL, &error)) {
        g_print("✓ Archivo leído: %s\n\n", argv[1]);
        
        // Tokenizar el archivo
        TokenList *tokens = tokenize_file(content);
        
        if (tokens) {
            // Imprimir todos los tokens
            token_list_print(tokens);
            
            // Estadísticas
            int tag_count = 0, class_count = 0, id_count = 0, text_count = 0;
            
            for (int i = 0; i < tokens->count; i++) {
                switch (tokens->tokens[i]->type) {
                    case TOKEN_TAG: tag_count++; break;
                    case TOKEN_CLASS: class_count++; break;
                    case TOKEN_ID: id_count++; break;
                    case TOKEN_TEXT: text_count++; break;
                    default: break;
                }
            }
            
            g_print("=== ESTADÍSTICAS ===\n");
            g_print("Total de tokens: %d\n", tokens->count);
            g_print("Tags: %d\n", tag_count);
            g_print("Clases: %d\n", class_count);
            g_print("IDs: %d\n", id_count);
            g_print("Textos: %d\n", text_count);
            g_print("====================\n");
            
            // Liberar memoria
            token_list_free(tokens);
        }
        
        g_free(content);
        
    } else {
        g_print("Error al leer el archivo: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    return 0;
}