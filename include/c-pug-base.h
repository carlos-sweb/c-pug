#ifndef C_PUG_BASE_H
#define C_PUG_BASE_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Tipos de tags HTML
typedef enum {
    TAG_VOID,      // Elementos auto-cerrados (img, br, meta, etc.)
    TAG_NORMAL,    // Elementos con contenido
    TAG_RAW        // Elementos que preservan whitespace (pre, code, etc.)
} HtmlTagType;

// Categorías de tags
typedef enum {
    CATEGORY_DOCUMENT,    // html, head, body
    CATEGORY_METADATA,    // meta, link, title, style
    CATEGORY_SECTIONS,    // header, footer, section, nav
    CATEGORY_HEADING,     // h1-h6
    CATEGORY_TEXT,        // p, span, strong, em
    CATEGORY_LISTS,       // ul, ol, li
    CATEGORY_TABLES,      // table, tr, td, th
    CATEGORY_FORMS,       // form, input, button
    CATEGORY_MEDIA,       // img, audio, video
    CATEGORY_EMBEDDED,    // iframe, embed, object
    CATEGORY_SCRIPTING,   // script, canvas
    CATEGORY_INTERACTIVE  // details, dialog
} HtmlTagCategory;

// Estructura para información completa de un tag HTML
typedef struct {
    const char* name;           // Nombre del tag
    HtmlTagType type;           // Tipo (void, normal, raw)
    HtmlTagCategory category;   // Categoría
    int has_attributes;         // Si puede tener atributos
    int can_nest;              // Si puede anidar otros elementos
    const char* description;    // Descripción en inglés
} HtmlTag;

// Array estático con todos los tags HTML
static const HtmlTag HTML_TAGS[] = {
    // === ELEMENTOS DE DOCUMENTO ===
    {"html",        TAG_NORMAL,    CATEGORY_DOCUMENT,    1, 1, "Root element"},
    {"head",        TAG_NORMAL,    CATEGORY_DOCUMENT,    1, 1, "Document metadata"},
    {"body",        TAG_NORMAL,    CATEGORY_DOCUMENT,    1, 1, "Document body"},
    
    // === METADATOS ===
    {"title",       TAG_NORMAL,    CATEGORY_METADATA,    1, 0, "Document title"},
    {"meta",        TAG_VOID,      CATEGORY_METADATA,    1, 0, "Metadata"},
    {"link",        TAG_VOID,      CATEGORY_METADATA,    1, 0, "External resource link"},
    {"style",       TAG_NORMAL,    CATEGORY_METADATA,    1, 0, "CSS styles"},
    
    // === SECCIONES ===
    {"div",         TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Content division"},
    {"span",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Inline container"},
    {"header",      TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Header content"},
    {"footer",      TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Footer content"},
    {"main",        TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Main content"},
    {"section",     TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Thematic section"},
    {"article",     TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Self-contained content"},
    {"aside",       TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Aside content"},
    {"nav",         TAG_NORMAL,    CATEGORY_SECTIONS,    1, 1, "Navigation links"},
    
    // === ENCABEZADOS ===
    {"h1",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 1"},
    {"h2",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 2"},
    {"h3",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 3"},
    {"h4",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 4"},
    {"h5",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 5"},
    {"h6",          TAG_NORMAL,    CATEGORY_HEADING,     1, 1, "Heading level 6"},
    
    // === TEXTO ===
    {"p",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Paragraph"},
    {"br",          TAG_VOID,      CATEGORY_TEXT,        1, 0, "Line break"},
    {"hr",          TAG_VOID,      CATEGORY_TEXT,        1, 0, "Horizontal rule"},
    {"pre",         TAG_RAW,       CATEGORY_TEXT,        1, 1, "Preformatted text"},
    {"blockquote",  TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Block quotation"},
    {"code",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Code fragment"},
    {"em",          TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Emphasized text"},
    {"strong",      TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Important text"},
    {"i",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Italic text"},
    {"b",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Bold text"},
    {"u",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Underlined text"},
    {"mark",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Marked/highlighted text"},
    {"small",       TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Small text"},
    {"sub",         TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Subscript"},
    {"sup",         TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Superscript"},
    
    // === LISTAS ===
    {"ul",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "Unordered list"},
    {"ol",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "Ordered list"},
    {"li",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "List item"},
    {"dl",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "Description list"},
    {"dt",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "Description term"},
    {"dd",          TAG_NORMAL,    CATEGORY_LISTS,       1, 1, "Description details"},
    
    // === ENLACES ===
    {"a",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Hyperlink"},
    
    // === IMÁGENES Y MEDIA ===
    {"img",         TAG_VOID,      CATEGORY_MEDIA,       1, 0, "Image"},
    {"picture",     TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Image container"},
    {"figure",      TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Figure with caption"},
    {"figcaption",  TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Figure caption"},
    {"audio",       TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Audio content"},
    {"video",       TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Video content"},
    {"source",      TAG_VOID,      CATEGORY_MEDIA,       1, 0, "Media source"},
    {"track",       TAG_VOID,      CATEGORY_MEDIA,       1, 0, "Text track"},
    
    // === TABLAS ===
    {"table",       TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table"},
    {"thead",       TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table header"},
    {"tbody",       TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table body"},
    {"tfoot",       TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table footer"},
    {"tr",          TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table row"},
    {"td",          TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table cell"},
    {"th",          TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table header cell"},
    {"caption",     TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table caption"},
    {"col",         TAG_VOID,      CATEGORY_TABLES,      1, 0, "Table column"},
    {"colgroup",    TAG_NORMAL,    CATEGORY_TABLES,      1, 1, "Table column group"},
    
    // === FORMULARIOS ===
    {"form",        TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Form"},
    {"input",       TAG_VOID,      CATEGORY_FORMS,       1, 0, "Form input"},
    {"textarea",    TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Multiline text input"},
    {"button",      TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Button"},
    {"select",      TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Dropdown list"},
    {"option",      TAG_NORMAL,    CATEGORY_FORMS,       1, 0, "Dropdown option"},
    {"optgroup",    TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Option group"},
    {"label",       TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Form label"},
    {"fieldset",    TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Field set"},
    {"legend",      TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Field set caption"},
    {"datalist",    TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Predefined options"},
    {"output",      TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Calculation output"},
    {"progress",    TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Progress bar"},
    {"meter",       TAG_NORMAL,    CATEGORY_FORMS,       1, 1, "Scalar measurement"},
    
    // === ELEMENTOS INTERACTIVOS ===
    {"details",     TAG_NORMAL,    CATEGORY_INTERACTIVE, 1, 1, "Disclosure widget"},
    {"summary",     TAG_NORMAL,    CATEGORY_INTERACTIVE, 1, 1, "Details summary"},
    {"dialog",      TAG_NORMAL,    CATEGORY_INTERACTIVE, 1, 1, "Dialog box"},
    {"menu",        TAG_NORMAL,    CATEGORY_INTERACTIVE, 1, 1, "Menu list"},
    
    // === CONTENIDO INCORPORADO ===
    {"iframe",      TAG_NORMAL,    CATEGORY_EMBEDDED,    1, 1, "Inline frame"},
    {"embed",       TAG_VOID,      CATEGORY_EMBEDDED,    1, 0, "External content"},
    {"object",      TAG_NORMAL,    CATEGORY_EMBEDDED,    1, 1, "Embedded object"},
    {"param",       TAG_VOID,      CATEGORY_EMBEDDED,    1, 0, "Object parameter"},
    
    // === SCRIPTING ===
    {"script",      TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 0, "JavaScript code"},
    {"canvas",      TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "Graphics canvas"},
    {"svg",         TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "SVG graphics"},
    {"math",        TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "MathML content"},
    
    // === ELEMENTOS SEMÁNTICOS HTML5 ===
    {"time",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Machine-readable time"},
    {"data",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Machine-readable data"},
    {"var",         TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Variable"},
    {"samp",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Sample output"},
    {"kbd",         TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Keyboard input"},
    {"s",           TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Strikethrough"},
    {"wbr",         TAG_VOID,      CATEGORY_TEXT,        1, 0, "Word break opportunity"},
    {"ruby",        TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Ruby annotation"},
    {"rt",          TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Ruby text"},
    {"rp",          TAG_NORMAL,    CATEGORY_TEXT,        1, 1, "Ruby parentheses"},
    
    // === OTROS ===
    {"map",         TAG_NORMAL,    CATEGORY_MEDIA,       1, 1, "Image map"},
    {"area",        TAG_VOID,      CATEGORY_MEDIA,       1, 0, "Image map area"},
    {"noscript",    TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "Content for non-JS users"},
    {"template",    TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "Template content"},
    {"slot",        TAG_NORMAL,    CATEGORY_SCRIPTING,   1, 1, "Shadow DOM slot"},
    
    // Fin del array
    {NULL, TAG_NORMAL, CATEGORY_DOCUMENT, 0, 0, NULL}
};

// Número total de tags (sin contar el NULL final)
#define HTML_TAGS_COUNT ((int)(sizeof(HTML_TAGS) / sizeof(HTML_TAGS[0])) - 1)

// ============================================================================
// FUNCIONES DE VALIDACIÓN
// ============================================================================

/**
 * Verifica si un tag es válido (existe en la lista de tags HTML)
 * @param tag_name Nombre del tag a verificar
 * @return 1 si es válido, 0 si no lo es
 */
static int is_valid_html_tag(const char* tag_name) {
    if (tag_name == NULL) return 0;
    
    for (int i = 0; i < HTML_TAGS_COUNT; i++) {
        if (strcmp(HTML_TAGS[i].name, tag_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Obtiene la información completa de un tag HTML
 * @param tag_name Nombre del tag
 * @return Puntero a HtmlTag o NULL si no se encuentra
 */
static const HtmlTag* get_html_tag_info(const char* tag_name) {
    if (tag_name == NULL) return NULL;
    
    for (int i = 0; i < HTML_TAGS_COUNT; i++) {
        if (strcmp(HTML_TAGS[i].name, tag_name) == 0) {
            return &HTML_TAGS[i];
        }
    }
    return NULL;
}

/**
 * Verifica si un tag es auto-cerrado (void element)
 * @param tag_name Nombre del tag
 * @return 1 si es auto-cerrado, 0 si no lo es
 */
static int is_void_tag(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    return tag ? (tag->type == TAG_VOID) : 0;
}

/**
 * Verifica si un tag puede contener otros elementos
 * @param tag_name Nombre del tag
 * @return 1 si puede anidar, 0 si no puede
 */
static int can_tag_nest(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    return tag ? tag->can_nest : 0;
}

/**
 * Verifica si un tag puede tener atributos
 * @param tag_name Nombre del tag
 * @return 1 si puede tener atributos, 0 si no puede
 */
static int can_tag_have_attributes(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    return tag ? tag->has_attributes : 0;
}

/**
 * Obtiene la categoría de un tag
 * @param tag_name Nombre del tag
 * @return Categoría del tag o CATEGORY_DOCUMENT si no se encuentra
 */
static HtmlTagCategory get_tag_category(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    return tag ? tag->category : CATEGORY_DOCUMENT;
}

/**
 * Obtiene el tipo de tag (void, normal, raw)
 * @param tag_name Nombre del tag
 * @return Tipo del tag o TAG_NORMAL si no se encuentra
 */
static HtmlTagType get_tag_type(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    return tag ? tag->type : TAG_NORMAL;
}

// ============================================================================
// FUNCIONES DE UTILIDAD
// ============================================================================

/**
 * Imprime información detallada de un tag
 * @param tag_name Nombre del tag
 */
static void print_tag_info(const char* tag_name) {
    const HtmlTag* tag = get_html_tag_info(tag_name);
    if (tag == NULL) {
        printf("❌ Tag '%s' no encontrado\n", tag_name);
        return;
    }
    
    const char* type_str = "Normal";
    if (tag->type == TAG_VOID) type_str = "Void";
    else if (tag->type == TAG_RAW) type_str = "Raw";
    
    const char* category_str = "Document";
    switch (tag->category) {
        case CATEGORY_METADATA: category_str = "Metadata"; break;
        case CATEGORY_SECTIONS: category_str = "Sections"; break;
        case CATEGORY_HEADING: category_str = "Heading"; break;
        case CATEGORY_TEXT: category_str = "Text"; break;
        case CATEGORY_LISTS: category_str = "Lists"; break;
        case CATEGORY_TABLES: category_str = "Tables"; break;
        case CATEGORY_FORMS: category_str = "Forms"; break;
        case CATEGORY_MEDIA: category_str = "Media"; break;
        case CATEGORY_EMBEDDED: category_str = "Embedded"; break;
        case CATEGORY_SCRIPTING: category_str = "Scripting"; break;
        case CATEGORY_INTERACTIVE: category_str = "Interactive"; break;
        default: category_str = "Document";
    }
    
    printf("🏷️  Tag: %s\n", tag->name);
    printf("   📝 Tipo: %s\n", type_str);
    printf("   📂 Categoría: %s\n", category_str);
    printf("   🔧 Atributos: %s\n", tag->has_attributes ? "Sí" : "No");
    printf("   🎯 Anidamiento: %s\n", tag->can_nest ? "Sí" : "No");
    printf("   📖 Descripción: %s\n", tag->description);
}

/**
 * Imprime todos los tags disponibles agrupados por categoría
 */
static void print_all_tags_by_category() {
    printf("📋 TODOS LOS TAGS HTML (%d total)\n", HTML_TAGS_COUNT);
    printf("========================================\n");
    
    HtmlTagCategory current_category = CATEGORY_DOCUMENT;
    const char* category_names[] = {
        "DOCUMENTO", "METADATOS", "SECCIONES", "ENCABEZADOS", "TEXTO",
        "LISTAS", "TABLAS", "FORMULARIOS", "MEDIA", "EMBEBIDOS",
        "SCRIPTING", "INTERACTIVOS"
    };
    
    for (int i = 0; i < HTML_TAGS_COUNT; i++) {
        if (HTML_TAGS[i].category != current_category) {
            current_category = HTML_TAGS[i].category;
            printf("\n📁 %s:\n", category_names[current_category]);
        }
        printf("   %-12s", HTML_TAGS[i].name);
        if (HTML_TAGS[i].type == TAG_VOID) printf(" (void)");
        printf("\n");
    }
}

#ifdef __cplusplus
}
#endif

#endif // C_PUG_BASA_H