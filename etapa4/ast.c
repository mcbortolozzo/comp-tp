#include "ast.h"

void print_type_name(int type);
void decompile(ast_node_t* node, int ident_level);
void decompile_children(ast_node_t** children, int ident_level);
void print_ident(int ident_level);

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
  node->line = getLineNumber();
  return node;
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

void ast_decompile(ast_node_t* node)
{
  decompile(node, 0);
}

void decompile_list_preppend(ast_node_t* node, int ident_level, char* separator)
{
  if(node->children[1])
  {
    decompile(node->children[1], ident_level);
    printf("%s", separator);
  }

  if(node->children[0])
    decompile(node->children[0], ident_level);
}

void decompile_list_append(ast_node_t* node, int ident_level, char* separator)
{
  if(node->children[0])
    decompile(node->children[0], ident_level);

  if(node->children[1])
  {
    printf("%s", separator);
    decompile(node->children[1], ident_level);
  }
}

void decompile_bin_expr(ast_node_t* node, int ident_level, char* op)
{
  decompile(node->children[0], ident_level);
  printf(" %s ", op);
  decompile(node->children[1], ident_level);
}

void decompile_expr(ast_node_t* node, int ident_level)
{
  printf("(");
  switch (node->type) {
    case AST_ID:                printf("%s", node->symbol->text); break;
    case AST_VECT:              printf("%s[", node->symbol->text);
                                decompile(node->children[0], ident_level);
                                printf("]");
                                break;
    case AST_LIT:               decompile(node->children[0], ident_level); break;
    case AST_CALL:              printf("%s(", node->symbol->text);
                                decompile(node->children[0], ident_level);
                                printf(")");
                                break;
    case AST_NOT:               printf("!");
                                decompile(node->children[0], ident_level);
                                break;
    case AST_ADD:               decompile_bin_expr(node, ident_level, "+"); break;
    case AST_SUB:               decompile_bin_expr(node, ident_level, "-"); break;
    case AST_MUL:               decompile_bin_expr(node, ident_level, "*"); break;
    case AST_DIV:               decompile_bin_expr(node, ident_level, "/"); break;
    case AST_LESS:              decompile_bin_expr(node, ident_level, "<"); break;
    case AST_GREATER:           decompile_bin_expr(node, ident_level, ">"); break;
    case AST_LE:                decompile_bin_expr(node, ident_level, "<="); break;
    case AST_GE:                decompile_bin_expr(node, ident_level, ">="); break;
    case AST_EQ:                decompile_bin_expr(node, ident_level, "=="); break;
    case AST_NE:                decompile_bin_expr(node, ident_level, "!="); break;
    case AST_AND:               decompile_bin_expr(node, ident_level, "&&"); break;
    case AST_OR:                decompile_bin_expr(node, ident_level, "||"); break;
  }
  printf(")");
}

void print_ident(int ident_level)
{
  int i;
  printf("\n");
  for(i = 0; i < ident_level; i++)
    printf("  ");
}

void decompile(ast_node_t* node, int ident_level)
{
  if(!node)
    return;


  int next_ident = ident_level;
  switch (node->type) {
    //external declarations
    case AST_PARAM_DECL:        //FALLTHROUGH
    case AST_VAR_DECL:          if(node->type == AST_VAR_DECL)
                                  print_ident(ident_level);
                                printf("%s: ", node->symbol->text);
                                decompile(node->children[0], ident_level);
                                decompile(node->children[1], ident_level);
                                if(node->type == AST_VAR_DECL)
                                  printf(";");
                                return; //manual decompile
    case AST_VAR_INIT:          printf(" = "); break;
    case AST_VECT_INIT:         printf("[%s] ", node->symbol->text); break;
    case AST_LIST_DECL:         break;
    case AST_LIST_VECT_INIT:    decompile_list_preppend(node, ident_level, " ");
                                return;

    //functions
    case AST_FUNC:              print_ident(ident_level); printf("\n"); break;
    case AST_FUNC_HEADER:       printf("(");
                                decompile(node->children[0], ident_level);
                                printf(") %s (", node->symbol->text);
                                if(node->children[1])
                                  decompile(node->children[1], ident_level);
                                printf(")");
                                return; //manual decompile
    case AST_LIST_PARAM_DECL:   decompile_list_preppend(node, ident_level, ", ");
                                return; //manual decompile
    //types
    case AST_BYTE:              printf("byte"); break;
    case AST_SHORT:             printf("short"); break;
    case AST_LONG:              printf("long"); break;
    case AST_FLOAT:             printf("float"); break;
    case AST_DOUBLE:            printf("double"); break;

    //comands
    case AST_CMD_BLOCK:         print_ident(ident_level);
                                printf("{");
                                decompile(node->children[0], ident_level + 1);
                                print_ident(ident_level);
                                printf("}");
                                return; //manual decompile
    case AST_CMD_LIST:
                                decompile_list_append(node, ident_level, ";");
                                return; //manual decompile
    case AST_ATTR:              print_ident(ident_level);
                                printf("%s", node->symbol->text);
                                if(node->children[1])
                                {
                                  printf("[");
                                  decompile(node->children[1], ident_level);
                                  printf("]");
                                }
                                printf(" = ");
                                decompile(node->children[0], ident_level);
                                return; //manual decompile
    case AST_READ:              print_ident(ident_level);
                                printf("read > %s", node->symbol->text);
                                break;
    case AST_PRINT:             print_ident(ident_level);
                                printf("print ");
                                break;
    case AST_LIST_PRINT:        decompile_list_preppend(node, ident_level, ", ");
                                return; //manual decompile
    case AST_RET:               print_ident(ident_level); printf("return "); break;

    //flow control
    case AST_IF:                print_ident(ident_level);
                                printf("if (");
                                decompile(node->children[0], ident_level);
                                printf(") then");
                                decompile(node->children[1], ident_level);
                                if(node->children[2])
                                {
                                  printf("else");
                                  decompile(node->children[2], ident_level);
                                }
                                return;
    case AST_WHILE:             print_ident(ident_level);
                                printf("while(");
                                decompile(node->children[0], ident_level);
                                printf(")");
                                decompile(node->children[1], ident_level);
                                return;
    case AST_LIST_ARG:          decompile_list_preppend(node, ident_level, ", ");
                                return;

    case AST_SYMBOL:            printf("%s", node->symbol->text); break;

    //expressions
    case AST_ID:                //FALLTHROUGH
    case AST_VECT:
    case AST_LIT:
    case AST_CALL:
    case AST_NOT:
    case AST_ADD:
    case AST_SUB:
    case AST_MUL:
    case AST_DIV:
    case AST_LESS:
    case AST_GREATER:
    case AST_LE:
    case AST_GE:
    case AST_EQ:
    case AST_NE:
    case AST_AND:
    case AST_OR:                decompile_expr(node, ident_level);
                                return; //manual decompile
  }

  decompile_children(node->children, next_ident);
}

void decompile_children(ast_node_t** children, int ident_level)
{
  int i;
  for(i = 0; i < MAX_CHILDREN; i++)
  {
    if(children[i])
      decompile(children[i], ident_level);
  }
}

void print_type_name(int type)
{
  switch (type) {
    case AST_LIST_DECL:         printf("LIST_DECL"); break;
    case AST_VAR_DECL:          printf("VAR_DECL"); break;
    case AST_VAR_INIT:          printf("LIST_INIT"); break;
    case AST_VECT_INIT:         printf("VECT_INIT"); break;
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
