#include "ast.h"

#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H

#define ID_SCALAR   200
#define ID_VECTOR   201
#define ID_FUNC     202

#define TYPE_INT   203
#define TYPE_FLOAT  205

int check_tree(ast_node_t *root);


#endif
