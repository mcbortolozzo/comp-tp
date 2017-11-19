#include "semantic_check.h"

int check_decl(ast_node_t *root);
int register_decl(ast_node_t *decl_node);
int register_func_param(ast_node_t *param_node);

int check_var(ast_node_t *root);
int check_var_init(ast_node_t *decl_node);

int is_declared(hash_node_t *symbol);
int get_semantic_type(ast_node_t *type_node);
void semantic_error(const char *error_msg, int line, const char *args, ...);

int check_tree(ast_node_t *root)
{
  if(!check_decl(root))
    printf("t");//exit(4);
  printf("test");
  if(!check_var(root))
    exit(4);
}

int check_decl(ast_node_t *root)
{
  int error = 0;
  ast_node_t *curr = root;
  while(curr)
  {
    if(curr->children[0]->type == AST_VAR_DECL)
      error |= register_decl(curr->children[0]);
    else if(curr->children[0]->type == AST_FUNC)
      error |= register_decl(curr->children[0]->children[0]);
    curr = curr->children[1];
  }
}

int register_decl(ast_node_t *decl_node)
{
  //printf("%s\n", decl_node->symbol->text);
  //printf("%d\n", decl_node->symbol->nature);
  if(is_declared(decl_node->symbol))
  {
    semantic_error("Variable \"%s\" already declared",
                      decl_node->line, decl_node->symbol->text);
    return -1;
  }
  else
  {
    decl_node->symbol->dataType = get_semantic_type(decl_node->children[0]);

    if(decl_node->type == AST_FUNC_HEADER)
    {
      decl_node->symbol->nature = ID_FUNC;
      register_func_param(decl_node->children[1]);
    }
    else
    {
      switch (decl_node->children[1]->type)
      {
        case AST_VECT_INIT: decl_node->symbol->nature = ID_VECTOR;
        case AST_VAR_INIT: decl_node->symbol->nature = ID_SCALAR;
      }
    }

    return 0;
  }
}

int register_func_param(ast_node_t *param_node)
{
  ast_node_t *curr = param_node;
  while(curr)
  {
    ast_node_t *param_decl = curr->children[0];
    printf("%s \n", param_decl->symbol->text);
    if(is_declared(param_decl->symbol))
    {
      semantic_error("Variable \"%s\" already declared",
                        param_decl->line, param_decl->symbol->text);
      return -1;
    }
    else
    {
      param_decl->symbol->dataType = get_semantic_type(param_decl->children[0]);
      param_decl->symbol->nature = ID_SCALAR;
    }
    curr = curr->children[1];
  }
  return 0;
}

int is_declared(hash_node_t *symbol)
{
  return symbol->nature != 0;
}

int get_semantic_type(ast_node_t *type_node)
{
  switch (type_node->type) {
    case AST_BYTE:
    case AST_SHORT:
    case AST_LONG:    return TYPE_INT;
    case AST_FLOAT:
    case AST_DOUBLE:  return TYPE_FLOAT;
  }
}

int get_symbol_type(hash_node_t *symbol)
{
  switch (symbol->type) {
    case SYMBOL_LIT_INT:  return TYPE_INT;
    case SYMBOL_LIT_REAL: return TYPE_FLOAT;
  }
}

int check_var(ast_node_t *root)
{
  ast_node_t *curr = root;
  int error = 0;
  while(curr)
  {
    /*if(curr->children[0]->type == AST_VAR_DECL) TODO n sei se precisa verificar init
      error |= check_decl_init(curr->children[0]);
    printf ("%d\n", error);*/

    if(curr->children[0]->type == AST_FUNC)
      error |= check_function(curr->children[0]);
    curr = curr->children[1];
  }
}

int check_function(ast_node_t *func_node)
{

}

int check_decl_init(ast_node_t *decl_node)
{
  int decl_type = decl_node->symbol->dataType;
  printf("%d\n", decl_type);

  if(decl_node->type == AST_VAR_INIT)
    return decl_type == get_symbol_type(decl_node->children[1]
                                                  ->children[0]->symbol);
  else
  {
    ast_node_t *vect_init = decl_node->children[0]->children[0];
    while(vect_init)
    {
      if(get_symbol_type(vect_init->children[0]->symbol) != decl_type)
        return -1;
      vect_init = vect_init->children[1];
    }
  }
}

semantic_error(const char *error_msg, int line, const char * args, ...)
{
  char buffer[100];
  sprintf(buffer, error_msg, args);
  printf("SEMANTIC ERROR (line %d): %s\n", line, buffer);
}
