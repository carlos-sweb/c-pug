#ifndef PUG_AST_H
#define PUG_AST_H

#include <glib.h>
#include <stdlib.h>
#include "pug/pug_tokens.h"
#include "pug/pug_attribute_list.h"

#ifdef __cplusplus
extern "C" {
#endif



// ============================================================================
// IMPLEMENTACIÓN INLINE - ASTNode
// ============================================================================
static inline ASTNode* ast_node_create(TokenType type, int depth) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
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
    node->children_capacity = 5;
    node->is_void = 0;
    node->is_inline = 0;
    node->is_block = 0;
    
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * node->children_capacity);
    if (!node->children) {
        free(node);
        return NULL;
    }
    
    return node;
}

static inline void ast_node_add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    
    if (parent->children_count >= parent->children_capacity) {
        parent->children_capacity *= 2;
        ASTNode** new_children = (ASTNode**)realloc(parent->children, 
                                                     sizeof(ASTNode*) * parent->children_capacity);
        if (!new_children) return;
        parent->children = new_children;
    }
    
    parent->children[parent->children_count] = child;
    child->parent = parent;
    parent->children_count++;
}

static inline void ast_node_free(ASTNode* node) {
    if (!node) return;
    
    if (node->tag) free(node->tag);
    if (node->text_content) free(node->text_content);
    if (node->id) free(node->id);
    
    if (node->classes) {
        for (int i = 0; i < node->class_count; i++) {
            if (node->classes[i]) free(node->classes[i]);
        }
        free(node->classes);
    }
    
    if (node->attributes) {
        attribute_list_free(node->attributes);
    }
    
    if (node->children) {
        for (int i = 0; i < node->children_count; i++) {
            ast_node_free(node->children[i]);
        }
        free(node->children);
    }
    
    free(node);
}

// ============================================================================
// IMPLEMENTACIÓN INLINE - NodeList
// ============================================================================
static inline NodeList* node_list_create() {
    NodeList* list = (NodeList*)malloc(sizeof(NodeList));
    if (!list) return NULL;
    
    list->capacity = 10;
    list->count = 0;
    list->nodes = (ASTNode**)malloc(sizeof(ASTNode*) * list->capacity);
    
    if (!list->nodes) {
        free(list);
        return NULL;
    }
    
    return list;
}

static inline void node_list_add(NodeList* list, ASTNode* node) {
    if (!list || !node) return;
    
    if (list->count >= list->capacity) {
        unsigned int new_capacity = list->capacity * 2;        
        ASTNode** new_nodes = (ASTNode**)realloc(list->nodes, sizeof(ASTNode*) * new_capacity);
        if (!new_nodes) return;
        list->capacity = new_capacity;
        list->nodes = new_nodes;
    }
    
    list->nodes[list->count] = node;
    list->count++;
}

static inline void node_list_free(NodeList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
        ast_node_free(list->nodes[i]);
    }
    
    free(list->nodes);
    free(list);
}

#ifdef __cplusplus
}
#endif

#endif // PUG_AST_H