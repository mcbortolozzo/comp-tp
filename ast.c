#include "ast.h"

void print_type_name(int type);

ast_node_t* ast_create(int type, hash_node_t* symbol, ast_node_t *child1,
  ast_node_t *child2, ast_node_t *child3, ast_node_t *child4)
{
  ast_node_t* node = calloc(1, sizeof(ast_node_t));
  node->type = type;
  node->symbol = symbol;
  node->children[0] = child1;
  node->children[1] = child2;
  node->children[2] = child3;
  node->children[3] = child4;
  return node;
}

void ast_print(ast_tree_t* tree)
{
  ast_print_node(tree->root, 0);
}

void ast_print_node(ast_node_t* node, int depth)
{
  int i;
  for(i = 0; i < depth; i++)
    printf("  ");

  print_type_name(node->type);
  printf(" ");
  if(node->symbol)
    printf("symbol: %s ", node->symbol->text);

  printf("\n");
  for(i = 0; i < MAX_CHILDREN; i++)
  {
    if(node->children[i])
      ast_print_node(node->children[i], depth + 1);
  }
}

void print_type_name(int type)
{
  switch (type) {
    case AST_LIST_DECL:         printf("LIST_DECL"); break;
    case AST_VAR_DECL:          printf("VAR_DECL"); break;
    case AST_VAR_INIT:          printf("LIST_INIT"); break;
    case AST_LIST_VECT_INIT:    printf("LIST_VECT_INIT"); break;
    case AST_FUNC:              printf("FUNC"); break;
    case AST_FUNC_HEADER:       printf("FUNC_HEADER"); break;
    case AST_LIST_PARAM_DECL:   printf("LIST_PARAM_DECL"); break;
    case AST_PARAM_DECL:        printf("PARAM_DECL"); break;
    case AST_CMD_LIST:          printf("CMD_LIST"); break;
    case AST_BYTE:              printf("BYTE"); break;
    case AST_SHORT:             printf("SHORT"); break;
    case AST_LONG:              printf("LONG"); break;
    case AST_FLOAT:             printf("FLOAT"); break;
    case AST_DOUBLE:            printf("DOUBLE"); break;
    case AST_ATTR:              printf("ATTRIBUTION"); break;
    case AST_READ:              printf("READ"); break;
    case AST_PRINT:             printf("PRINT"); break;
    case AST_LIST_PRINT:        printf("LIST_PRINT"); break;
    case AST_RET:               printf("RETURN"); break;
    case AST_IF:                printf("IF"); break;
    case AST_WHILE:             printf("WHILE"); break;
    case AST_VECT:              printf("VECT"); break;
    case AST_ID:                printf("ID"); break;
    case AST_LIT:               printf("LIT"); break;
    case AST_CALL:              printf("CALL"); break;
    case AST_NOT:               printf("NOT"); break;
    case AST_ADD:               printf("ADD"); break;
    case AST_SUB:               printf("SUB"); break;
    case AST_MUL:               printf("MUL"); break;
    case AST_DIV:               printf("DIV"); break;
    case AST_LESS:              printf("LESS"); break;
    case AST_GREATER:           printf("GREATER"); break;
    case AST_LE:                printf("LE"); break;
    case AST_GE:                printf("GE"); break;
    case AST_EQ:                printf("EQ"); break;
    case AST_NE:                printf("NE"); break;
    case AST_AND:               printf("AND"); break;
    case AST_OR:                printf("OR"); break;
    case AST_LIST_ARG:          printf("LIST_ARG"); break;
    case AST_SYMBOL:            printf("SYMBOL"); break;
  }
}
