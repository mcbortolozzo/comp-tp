#include <stdio.h>

#include "hash.h"

#ifndef AST_H
#define AST_H

#define MAX_CHILDREN        4

#define AST_CMD_BLOCK       4
#define AST_LIST_DECL       5
#define AST_VAR_DECL        6
#define AST_VAR_INIT        7
#define AST_VECT_INIT       8
#define AST_LIST_VECT_INIT  9
#define AST_FUNC            10
#define AST_FUNC_HEADER     11
#define AST_LIST_PARAM_DECL 12
#define AST_PARAM_DECL      13
#define AST_CMD_LIST        14
#define AST_BYTE            15
#define AST_SHORT           16
#define AST_LONG            17
#define AST_FLOAT           18
#define AST_DOUBLE          19
#define AST_ATTR            20
#define AST_READ            21
#define AST_PRINT           22
#define AST_LIST_PRINT      23
#define AST_RET             24
#define AST_IF              25
#define AST_WHILE           26
#define AST_ID              30
#define AST_VECT            31
#define AST_LIT             32
#define AST_CALL            33
#define AST_NOT             34
#define AST_ADD             35
#define AST_SUB             36
#define AST_MUL             37
#define AST_DIV             38
#define AST_LESS            39
#define AST_GREATER         40
#define AST_LE              41
#define AST_GE              42
#define AST_EQ              43
#define AST_NE              44
#define AST_AND             45
#define AST_OR              46
#define AST_LIST_ARG        50
#define AST_SYMBOL          55

typedef struct ast_node {
  int type;
  hash_node_t* symbol;
  struct ast_node* children[MAX_CHILDREN];
  int line;
} ast_node_t;

ast_node_t* ast_create(int type, hash_node_t* symbol, ast_node_t *child1,
  ast_node_t *child2, ast_node_t *child3, ast_node_t *child4);

void ast_print_node(ast_node_t* node, int depth);

void ast_decompile(ast_node_t* node);

#endif
