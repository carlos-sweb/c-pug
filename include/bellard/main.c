#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libregexp.h"

/* Implementaciones requeridas por libregexp */
int lre_check_stack_overflow(void *opaque, size_t alloca_size) {
    /* Retornar 0 = OK, no-cero = overflow */
    return 0;
}

int lre_check_timeout(void *opaque) {
    /* Retornar 0 = OK, no-cero = timeout */
    return 0;
}

void *lre_realloc(void *opaque, void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

/* Función auxiliar para imprimir matches */
void print_match(const char *input, uint8_t *start, uint8_t *end) {
    if (!start || !end) {
        printf("<no match>");
        return;
    }
    
    int len = end - start;
    printf("'%.*s' (pos: %ld, len: %d)", 
           len, (char *)start, start - (uint8_t *)input, len);
}

/* Ejemplo 1: Match simple */
void ejemplo_match_simple() {
    printf("\n=== EJEMPLO 1: Match Simple ===\n");
    
    const char *pattern = "hello";
    const char *input = "hello world";
    
    uint8_t *bc;
    int bc_len;
    char error_msg[128];
    
    /* Compilar la expresión regular */
    bc = lre_compile(&bc_len, error_msg, sizeof(error_msg),
                     pattern, strlen(pattern), 0, NULL);
    
    if (!bc) {
        printf("Error compilando: %s\n", error_msg);
        return;
    }
    
    /* Preparar buffer para capturas */
    int capture_count = lre_get_capture_count(bc);
    uint8_t *capture[capture_count * 2];
    
    /* Ejecutar regex */
    int ret = lre_exec(capture, bc, (uint8_t *)input, 0, strlen(input), 0, NULL);
    
    if (ret == 1) {
        printf("Match encontrado: ");
        print_match(input, capture[0], capture[1]);
        printf("\n");
    } else {
        printf("No match\n");
    }
    
    free(bc);
}

/* Ejemplo 2: Grupos de captura */
void ejemplo_grupos_captura() {
    printf("\n=== EJEMPLO 2: Grupos de Captura ===\n");
    
    const char *pattern = "(\\w+)@(\\w+\\.\\w+)";
    const char *input = "carlos@swb.cl";
    
    uint8_t *bc;
    int bc_len;
    char error_msg[128];
    
    bc = lre_compile(&bc_len, error_msg, sizeof(error_msg),
                     pattern, strlen(pattern), 0, NULL);
    
    if (!bc) {
        printf("Error: %s\n", error_msg);
        return;
    }
    
    int capture_count = lre_get_capture_count(bc);
    uint8_t *capture[capture_count * 2];
    
    int ret = lre_exec(capture, bc, (uint8_t *)input, 0, strlen(input), 0, NULL);
    
    if (ret == 1) {
        printf("Email completo: ");
        print_match(input, capture[0], capture[1]);
        printf("\n");
        
        printf("Usuario: ");
        print_match(input, capture[2], capture[3]);
        printf("\n");
        
        printf("Dominio: ");
        print_match(input, capture[4], capture[5]);
        printf("\n");
    }
    
    free(bc);
}

/* Ejemplo 3: Case insensitive */
void ejemplo_case_insensitive() {
    printf("\n=== EJEMPLO 3: Case Insensitive ===\n");
    
    const char *pattern = "hello";
    const char *input = "HELLO World";
    
    uint8_t *bc;
    int bc_len;
    char error_msg[128];
    
    /* LRE_FLAG_IGNORECASE para ignorar mayúsculas/minúsculas */
    bc = lre_compile(&bc_len, error_msg, sizeof(error_msg),
                     pattern, strlen(pattern), LRE_FLAG_IGNORECASE, NULL);
    
    if (!bc) {
        printf("Error: %s\n", error_msg);
        return;
    }
    
    int capture_count = lre_get_capture_count(bc);
    uint8_t *capture[capture_count * 2];
    
    int ret = lre_exec(capture, bc, (uint8_t *)input, 0, strlen(input), 0, NULL);
    
    if (ret == 1) {
        printf("Match (ignorando case): ");
        print_match(input, capture[0], capture[1]);
        printf("\n");
    }
    
    free(bc);
}

/* Ejemplo 4: Uso práctico para template Pug */
void ejemplo_template_pug() {
    printf("\n=== EJEMPLO 4: Template Pug - Variables ===\n");
    
    /* Detectar variables tipo #{variable} */
    const char *pattern = "#\\{([a-zA-Z_][a-zA-Z0-9_]*)\\}";
    const char *input = "Hello #{name}, you have #{count} messages";
    
    uint8_t *bc;
    int bc_len;
    char error_msg[128];
    
    bc = lre_compile(&bc_len, error_msg, sizeof(error_msg),
                     pattern, strlen(pattern), 0, NULL);
    
    if (!bc) {
        printf("Error: %s\n", error_msg);
        return;
    }
    
    int capture_count = lre_get_capture_count(bc);
    uint8_t *capture[capture_count * 2];
    
    /* Buscar todas las ocurrencias */
    int pos = 0;
    int input_len = strlen(input);
    int match_num = 0;
    
    while (pos < input_len) {
        int ret = lre_exec(capture, bc, (uint8_t *)input, pos, input_len, 0, NULL);
        
        if (ret != 1) break;
        
        match_num++;
        printf("Variable %d: ", match_num);
        print_match(input, capture[2], capture[3]);
        printf(" en posición %ld\n", capture[0] - (uint8_t *)input);
        
        /* Avanzar después del match */
        pos = (capture[1] - (uint8_t *)input);
    }
    
    free(bc);
}

/* Ejemplo 5: Detectar tags Pug */
void ejemplo_pug_tags() {
    printf("\n=== EJEMPLO 5: Template Pug - Tags ===\n");
    
    /* Detectar tags HTML tipo: div.clase#id */
    const char *pattern = "^([a-z]+)(\\.([a-zA-Z0-9_-]+))?(#([a-zA-Z0-9_-]+))?";
    const char *input = "div.container#main";
    
    uint8_t *bc;
    int bc_len;
    char error_msg[128];
    
    bc = lre_compile(&bc_len, error_msg, sizeof(error_msg),
                     pattern, strlen(pattern), 0, NULL);
    
    if (!bc) {
        printf("Error: %s\n", error_msg);
        return;
    }
    
    int capture_count = lre_get_capture_count(bc);
    uint8_t *capture[capture_count * 2];
    
    int ret = lre_exec(capture, bc, (uint8_t *)input, 0, strlen(input), 0, NULL);
    
    if (ret == 1) {
        printf("Tag: ");
        print_match(input, capture[2], capture[3]);
        printf("\n");
        
        if (capture[6]) {
            printf("Clase: ");
            print_match(input, capture[6], capture[7]);
            printf("\n");
        }
        
        if (capture[10]) {
            printf("ID: ");
            print_match(input, capture[10], capture[11]);
            printf("\n");
        }
    }
    
    free(bc);
}

int main() {
    printf("Ejemplos de uso de libregexp para template engine Pug\n");
    printf("====================================================\n");
    
    ejemplo_match_simple();
    ejemplo_grupos_captura();
    ejemplo_case_insensitive();
    ejemplo_template_pug();
    ejemplo_pug_tags();
    
    return 0;
}
