#include <glib-2.0/glib.h>
#include <stdio.h>
#include <ctype.h>
#include <c-pug.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        g_print("Uso: %s <archivo.pug>\n", argv[0]);
        return 1;
    }
    
    gchar *content = NULL;
    GError *error = NULL;
    
    if (g_file_get_contents(argv[1], &content, NULL, &error)) {
        g_print("El Archivo leido es: %s\n\n", argv[1]);
        
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
            
            
            g_print("=== ESTADISTICAS ===\n");
            g_print("Total de tokens: %d\n", tokens->count);
            g_print("Tags: %d\n", tag_count);
            g_print("Clases: %d\n", class_count);
            g_print("IDs: %d\n", id_count);
            g_print("Textos: %d\n", text_count);
            g_print("====================\n");
            
            // Liberar memoria
            token_list_free(tokens);
        }


    char* html = process_pug_file(content, 0, 2 , 0 );
    if (html) {
        printf("=== HTML Output ===\n%s\n", html);
    render_free(html);
    } else {
        printf("Error procesando el archivo Pug\n");
    }
        
        g_free(content);
        
    } else {
        g_print("Error al leer el archivo: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    
    return 0;
}