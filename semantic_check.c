#include "semantic_check.h"

void check_decl(ast_node_t *root);
void register_decl(ast_node_t *decl_node);
void register_func_param(ast_node_t *param_node, hash_node_t* symbol);
int check_decl_init(ast_node_t *decl_node);

void check_var(ast_node_t *root);
void check_var_init(ast_node_t *decl_node);
void check_function(ast_node_t *func_node);
void check_cmd(ast_node_t *cmd_node, func_decl_t *local_scope);
int check_expr(ast_node_t *expr_node, func_decl_t *local_scope); //returns type
void check_func_params(ast_node_t *param_list, func_decl_t *local_scope, hash_node_t *local_symbol);

int is_declared(hash_node_t *symbol);
int get_semantic_type(ast_node_t *type_node);
void semantic_error(const char *error_msg, int line, const char *args, ...);

int sem_error = 0;

int check_tree(ast_node_t *root)
{
  scope_list = create_func_list();
  check_decl(root);
  check_var(root);
  if(sem_error)
  {
    printf("Semantic errors found, compilation failed\n");
    exit(4);
  }
}

void check_decl(ast_node_t *root)
{
  ast_node_t *curr = root;
  while(curr)
  {
    if(curr->children[0]->type == AST_VAR_DECL)
      register_decl(curr->children[0]);
    else if(curr->children[0]->type == AST_FUNC)
      register_decl(curr->children[0]->children[0]);
    curr = curr->children[1];
  }
}

void register_decl(ast_node_t *decl_node)
{
  //printf("%s\n", decl_node->symbol->text);
  //printf("%d\n", decl_node->symbol->nature);
  if(is_declared(decl_node->symbol))
  {
    semantic_error("Variable \"%s\" already declared",
                      decl_node->line, decl_node->symbol->text);
  }
  else
  {
    decl_node->symbol->dataType = get_semantic_type(decl_node->children[0]);

    if(decl_node->type == AST_FUNC_HEADER)
    {
      decl_node->symbol->nature = ID_FUNC;
      register_func_param(decl_node->children[1], decl_node->symbol);
    }
    else
    {
      switch (decl_node->children[1]->type)
      {
        case AST_VECT_INIT: decl_node->symbol->nature = ID_VECTOR; break;
        case AST_VAR_INIT: decl_node->symbol->nature = ID_SCALAR; break;
      }
    }
  }
}

void register_func_param(ast_node_t *param_node, hash_node_t *func_symbol)
{
  ast_node_t *curr = param_node;
  func_decl_t *scope = create_func_decl(func_symbol);
  while(curr)
  {
    ast_node_t *param_decl = curr->children[0];
    // check if param already declared in this scope
    // + check if it's either undefined or exclusively parameter type on global
    // scope
    hash_node_t *symbol_prev_decl = find_param(scope, param_decl->symbol->text);
    if(symbol_prev_decl ||
        (param_decl->symbol->nature != ID_UNDEFINED
          && param_decl->symbol->nature != ID_PARAM))
    {
      semantic_error("Variable \"%s\" already declared",
                        param_decl->line, param_decl->symbol->text);
    }
    else
    {
      hash_node_t *symbol = param_decl->symbol;
      hash_node_t *scope_symbol = insert_param(scope, symbol);
      scope_symbol->nature = ID_SCALAR;
      scope_symbol->dataType = get_semantic_type(param_decl->children[0]);
      param_decl->symbol->nature = ID_PARAM;
    }
    curr = curr->children[1];
  }
  insert_decl(scope_list, scope);
}

int is_declared(hash_node_t *symbol)
{
  return symbol->nature != ID_UNDEFINED;
}

int get_semantic_type(ast_node_t *type_node)
{
  switch (type_node->type) {
    case AST_BYTE:
    case AST_SHORT:
    case AST_LONG:    return TYPE_INT;
    case AST_FLOAT:
    case AST_DOUBLE:  return TYPE_FLOAT;
    default:          return TYPE_UNDEFINED;
  }
}

int get_symbol_type(hash_node_t *symbol)
{
  int type = 0;
  switch (symbol->type) {
    case SYMBOL_LIT_CHAR:
    case SYMBOL_LIT_INT:  type = TYPE_INT; break;
    case SYMBOL_LIT_REAL: type = TYPE_FLOAT; break;
    default:              type = TYPE_UNDEFINED; break;
  }
  symbol->dataType = type;
  return type;
}

void check_var(ast_node_t *root)
{
  ast_node_t *curr = root;
  while(curr)
  {
    if(curr->children[0]->type == AST_VAR_DECL)
      check_decl_init(curr->children[0]);

    if(curr->children[0]->type == AST_FUNC)
      check_function(curr->children[0]);
    curr = curr->children[1];
  }
}

void check_function(ast_node_t *func_node)
{
  func_decl_t *scope = find_decl(scope_list, func_node->children[0]->symbol);
  check_cmd(func_node->children[1], scope);
}

void check_cmd(ast_node_t *cmd_node, func_decl_t *local_scope)
{
  int expr_type;
  ast_node_t *curr;

  if(!cmd_node)
    return;


  switch (cmd_node->type) {
    case AST_ATTR:
      expr_type = check_expr(cmd_node->children[0], local_scope);
      if(cmd_node->symbol->nature == ID_UNDEFINED)
        semantic_error("Undeclared variable %s",
                      cmd_node->line, cmd_node->symbol->text);
      if(cmd_node->children[1]) //this means it's vector attr
      {
        if(cmd_node->symbol->nature != ID_VECTOR)
          semantic_error("Variable %s is not a vector", cmd_node->line,
                            cmd_node->symbol->text);
        int index_type = check_expr(cmd_node->children[1], local_scope);
        if(index_type != TYPE_INT)
          semantic_error("Vector index must be Integer", cmd_node->line, 0);
      }
      else
      {
          if(cmd_node->symbol->nature != ID_SCALAR)
            semantic_error("Assign operation incompatible with variable %s",
                            cmd_node->line, cmd_node->symbol->text);
      }
      if(expr_type != cmd_node->symbol->dataType)
        semantic_error("Incompatible type on attribution of variable %s",
                          cmd_node->line, cmd_node->symbol->text);
      break;

    case AST_READ:
      if(cmd_node->symbol->nature == ID_UNDEFINED)
        semantic_error("Undeclared variable %s",
                      cmd_node->line, cmd_node->symbol->text);
      if(cmd_node->symbol->nature != ID_SCALAR)
        semantic_error("Invalid read operation on non-scalar variable %s",
                        cmd_node->line, cmd_node->symbol->text);
      break;

    case AST_PRINT:
      curr = cmd_node->children[0];
      while(curr)
      {
        if(curr->children[0]->type != AST_SYMBOL)
          check_expr(curr->children[0], local_scope);
        curr = curr->children[1];
      }
      break;

    case AST_RET:
      expr_type = check_expr(cmd_node->children[0], local_scope);
      if(expr_type != local_scope->id->dataType)
        semantic_error("Incompatible type on function return", cmd_node->line, 0);
      break;

    case AST_WHILE: //FALLTHROUGH
    case AST_IF:
      expr_type = check_expr(cmd_node->children[0], local_scope);
      if(expr_type != TYPE_BOOL)
        semantic_error("Incompatible non bool value on condition", cmd_node->line, 0);
      check_cmd(cmd_node->children[1], local_scope);
      if(cmd_node->children[2])
        check_cmd(cmd_node->children[2], local_scope);
      break;

    case AST_CMD_BLOCK:
      curr = cmd_node->children[0];
      while(curr)
      {
        check_cmd(curr->children[0], local_scope);
        curr = curr->children[1];
      }
      break;
  }
}

int check_expr(ast_node_t *expr_node, func_decl_t *local_scope)
{
  int type1, type2;
  hash_node_t* local_symbol;
  if(expr_node->symbol && expr_node->symbol->nature == ID_PARAM)
  {
    local_symbol = find_param(local_scope, expr_node->symbol->text);

    if(!local_symbol)
    {
      semantic_error("Undeclared variable %s",
          expr_node->line, expr_node->symbol->text);
      return TYPE_UNDEFINED;
    }
  }
  else
    local_symbol = expr_node->symbol;


  switch (expr_node->type) {
    case AST_ID:
      if(local_symbol->nature == ID_UNDEFINED)
        semantic_error("Undeclared variable %s",
                          expr_node->line, expr_node->symbol->text);
      else if(local_symbol->nature != ID_SCALAR)
        semantic_error("Wrong usage of variable %s",
                      expr_node->line, expr_node->symbol->text);
      return local_symbol->dataType;

    case AST_VECT:
      if(local_symbol->nature == ID_UNDEFINED)
        semantic_error("Undeclared variable %s",
                          expr_node->line, expr_node->symbol->text);
      else if(local_symbol->nature != ID_VECTOR)
        semantic_error("Wrong usage of variable %s",
                      expr_node->line, expr_node->symbol->text);
      else if(check_expr(expr_node->children[0], local_scope) != TYPE_INT)
        semantic_error("Vector index must be Integer", expr_node->line, 0);
      return local_symbol->dataType;

    case AST_LIT:
      expr_node->children[0]->symbol->dataType = get_symbol_type(expr_node->children[0]->symbol);
      return expr_node->children[0]->symbol->dataType;

    case AST_CALL:
      if(local_symbol->nature == ID_UNDEFINED)
        semantic_error("Undeclared variable %s",
                        expr_node->line, expr_node->symbol->text);
      else if(local_symbol->nature != ID_FUNC)
        semantic_error("Wrong usage of variable %s",
                      expr_node->line, expr_node->symbol->text);

      check_func_params(expr_node->children[0], local_scope, local_symbol);

      return local_symbol->dataType;

      case AST_NOT:
        if(check_expr(expr_node->children[0], local_scope) != TYPE_BOOL)
          semantic_error("Wrong variable type, expected bool",
                          expr_node->line, 0);
        return TYPE_BOOL;

      case AST_ADD:
      case AST_SUB:
      case AST_MUL:
      case AST_DIV:
      type1 = check_expr(expr_node->children[0], local_scope);
      type2 = check_expr(expr_node->children[1], local_scope);
      if(type1 == TYPE_BOOL || type2 == TYPE_BOOL)
        semantic_error("Invalid expression, expected float or integer",
                          expr_node->line, 0);
      else if(type1 == TYPE_FLOAT || type2 == TYPE_FLOAT)
        return TYPE_FLOAT;
      else if(type1 != TYPE_UNDEFINED && type2 != TYPE_UNDEFINED)
        return TYPE_INT;
      else
        return TYPE_UNDEFINED;
      case AST_LESS:
      case AST_GREATER:
      case AST_LE:
      case AST_GE:
      case AST_EQ:
      case AST_NE:
      type1 = check_expr(expr_node->children[0], local_scope);
      type2 = check_expr(expr_node->children[1], local_scope);
      if(type1 == TYPE_BOOL || type2 == TYPE_BOOL)
        semantic_error("Invalid expression, expected float or integer",
                          expr_node->line, 0);
      else if(type1 != TYPE_UNDEFINED && type2 != TYPE_UNDEFINED)
        return TYPE_BOOL;
      else
        return TYPE_UNDEFINED;
      case AST_AND:
      case AST_OR:
      type1 = check_expr(expr_node->children[0], local_scope);
      type2 = check_expr(expr_node->children[1], local_scope);
      if(type1 == TYPE_UNDEFINED || type2 == TYPE_UNDEFINED)
        return TYPE_UNDEFINED;
      else if(type1 != TYPE_BOOL || type2 != TYPE_BOOL)
        semantic_error("Invalid expression, expected bool",
                          expr_node->line, 0);
      else
        return TYPE_BOOL;
  }

}

int check_decl_init(ast_node_t *decl_node)
{
  int decl_type = decl_node->symbol->dataType;

  if(decl_node->children[1]->type == AST_VAR_INIT)
  {
    if(decl_type != get_symbol_type(decl_node->children[1]->children[0]->symbol))
      semantic_error("Wrong type on variable initialization",
                        decl_node->line, 0);
  }
  else
  {
    ast_node_t *vect_init = decl_node->children[1]->children[0];
    while(vect_init)
    {
      if(get_symbol_type(vect_init->children[0]->symbol) != decl_type)
        semantic_error("Wrong type on variable initialization",
                        decl_node->line, 0);
      vect_init = vect_init->children[1];
    }
  }
}

void check_func_params(ast_node_t *param_list, func_decl_t *local_scope, hash_node_t *func_symbol)
{
  func_decl_t *called_function = find_decl(scope_list, func_symbol);
  ast_node_t *actual_param = param_list;
  hash_node_t *expected_param = called_function->params;
  while(actual_param)
  {
    if(!expected_param)
    {
      semantic_error("Parameter count mismatch",
                        param_list->line, 0);
      return;
    }
    int param_type = check_expr(actual_param->children[0], local_scope);
    if(param_type != expected_param->dataType)
    {
      semantic_error("Parameter type mismatch",
                        param_list->line, 0);
    }
    expected_param = expected_param->next;
    actual_param = actual_param->children[1];
  }

  if(expected_param)
    semantic_error("Parameter count mismatch",
                    param_list->line, 0);

}

void semantic_error(const char *error_msg, int line, const char * args, ...)
{
  char buffer[100];
  sprintf(buffer, error_msg, args);
  printf("SEMANTIC ERROR (line %d): %s\n", line, buffer);
  sem_error = 1;
}
