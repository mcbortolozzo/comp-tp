#include "ast.h"
#include "func_list.h"

#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H

#define ID_UNDEFINED    199
#define ID_SCALAR       200
#define ID_VECTOR       201
#define ID_FUNC         202
#define ID_PARAM        203

#define TYPE_UNDEFINED  205
#define TYPE_INT        210
#define TYPE_FLOAT      215
#define TYPE_BOOL       220

int check_tree(ast_node_t *root);
func_list_t* scope_list;

#endif
