#include "tac.h"

int Label_Count = 0;
int Temp_Count = 0;

hash_node_t* make_temp();
hash_node_t* make_label();

tac_node_t *tac_attr(ast_node_t *ast_node);
tac_node_t *tac_expr(ast_node_t *ast_node);
int get_expr_tac(int ast_type);

tac_node_t *tac_param_decl(ast_node_t *ast_node);
tac_node_t *tac_cmd_list(ast_node_t *ast_node);

tac_node_t *tac_vec(ast_node_t *ast_node);
tac_node_t *tac_call(ast_node_t *ast_node);
tac_node_t *tac_arg_list(ast_node_t *ast_node);

tac_node_t *tac_ret(ast_node_t *ast_node);

void tac_print_node(tac_node_t *tac_node);

tac_node_t *tac_attr(ast_node_t *ast_node)
{
  tac_node_t *val = tac_gen(ast_node->children[0]);
  hash_node_t *res = ast_node->symbol;
  tac_node_t *new_tac;
  if(ast_node->children[1])
  {
    tac_node_t *index = tac_gen(ast_node->children[1]);
    val = tac_join(index, val);
    new_tac = tac_new(TAC_TOVECMOV, res, index->res, val->res);
  }
  else
  {
    new_tac = tac_new(TAC_MOV, res, val->res, 0);
  }
  return tac_join(val, new_tac);
}

tac_node_t *tac_expr(ast_node_t *ast_node)
{
  tac_node_t *op1 = tac_gen(ast_node->children[0]);
  tac_node_t *op2 = tac_gen(ast_node->children[1]);
  hash_node_t *res = make_temp();
  int type = get_expr_tac(ast_node->type);
  tac_node_t *new_tac = tac_new(type, res,  op1 ? op1->res : 0,
                                            op2 ? op2->res : 0);
  return tac_join(tac_join(op1, op2), new_tac);
}

int get_expr_tac(int ast_type)
{
  switch (ast_type) {
    case AST_ADD:     return TAC_ADD;
    case AST_SUB:     return TAC_SUB;
    case AST_MUL:     return TAC_MUL;
    case AST_DIV:     return TAC_DIV;
    case AST_LESS:    return TAC_LESS;
    case AST_GREATER: return TAC_GREATER;
    case AST_LE:      return TAC_LE;
    case AST_GE:      return TAC_GE;
    case AST_EQ:      return TAC_EQ;
    case AST_NE:      return TAC_NE;
    case AST_AND:     return TAC_AND;
    case AST_OR:      return TAC_OR;
  }
  return TAC_UNDEFINED;
}

tac_node_t *tac_vec(ast_node_t* ast_node)
{
  tac_node_t *index = tac_gen(ast_node->children[0]);
  hash_node_t *vec_temp = make_temp();
  tac_node_t *vec_node
    = tac_new(TAC_FROMVECMOV, vec_temp, index->res, ast_node->symbol);
  return tac_join(index, vec_node);
}

tac_node_t *tac_param_decl(ast_node_t *ast_node)
{
  tac_node_t *param_list = 0;
  if(ast_node->children[1])
    param_list = tac_gen(ast_node->children[1]);

  tac_node_t *new_tac = tac_new(TAC_POPARG,ast_node->children[0]->symbol, 0, 0);
  tac_node_t* res = tac_join(new_tac, param_list);
  return res;
}

tac_node_t *tac_cmd_list(ast_node_t *ast_node)
{
  tac_node_t *cmd_list = 0;
  if(ast_node->children[1])
    cmd_list = tac_gen(ast_node->children[1]);

  return tac_join(tac_gen(ast_node->children[0]), cmd_list);
}

tac_node_t *tac_ret(ast_node_t *ast_node)
{
  tac_node_t *tac_expr = tac_gen(ast_node->children[0]);
  tac_node_t *new_tac = tac_new(TAC_RET, 0, tac_expr ? tac_expr->res : 0, 0);
  return tac_join(tac_expr, new_tac);
}

tac_node_t *tac_print_list(ast_node_t *ast_node)
{
  tac_node_t *print_list = 0;
  if(ast_node->children[1])
    print_list = tac_gen(ast_node->children[1]);

  tac_node_t *print_arg = tac_gen(ast_node->children[0]);
  tac_node_t *new_tac = tac_new(TAC_PRINT, 0, print_arg ? print_arg->res:0, 0);
  new_tac = tac_join(print_arg, new_tac);
  return tac_join(new_tac, print_list);
}

tac_node_t *tac_func(ast_node_t *ast_node)
{
  tac_node_t *tac_label
    = tac_new(TAC_LABEL, 0, ast_node->children[0]->symbol, 0);
  tac_node_t *func
    = tac_join(tac_gen(ast_node->children[0]),tac_gen(ast_node->children[1]));
  func = tac_join(tac_label, func);
  tac_node_t *ret = tac_new(TAC_RET, 0, 0, 0);
  return tac_join(func, ret);
}

tac_node_t *tac_call(ast_node_t *ast_node)
{
  tac_node_t *call_args = tac_gen(ast_node->children[0]);
  tac_node_t *call = tac_new(TAC_CALL, 0, ast_node->symbol, 0);
  call = tac_join(call_args, call);
  hash_node_t *res = make_temp();
  tac_node_t *tac_res = tac_new(TAC_POPARG, res, 0, 0);
  return tac_join(call, tac_res);
}

tac_node_t *tac_arg_list(ast_node_t *ast_node)
{
  tac_node_t *arg_list = 0;
  if(ast_node->children[1])
    arg_list = tac_gen(ast_node->children[1]);

  tac_node_t *arg_expr = tac_gen(ast_node->children[0]);
  tac_node_t *arg = tac_new(TAC_PUSHARG, 0, arg_expr ? arg_expr->res : 0, 0);
  arg = tac_join(arg_expr, arg);

  return tac_join(arg_list, arg);
}


tac_node_t* tac_gen(ast_node_t *ast_node)
{
  tac_node_t* tac_node;
  if(!ast_node)
    return 0;

  switch (ast_node->type) {
    case AST_ATTR:
      return tac_attr(ast_node);
    case AST_ADD: //FALLTHROUGH
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
    case AST_OR:
      return tac_expr(ast_node);
    case AST_FUNC:
      return tac_func(ast_node);
    case AST_FUNC_HEADER:
      return tac_gen(ast_node->children[1]);
    case AST_LIST_PARAM_DECL:
      return tac_param_decl(ast_node);
    case AST_CMD_LIST:
      return tac_cmd_list(ast_node);
    case AST_CMD_BLOCK:
      return tac_gen(ast_node->children[0]);
    case AST_READ:
      return tac_new(TAC_READ, ast_node->symbol, 0, 0);
    case AST_PRINT:
      return tac_gen(ast_node->children[0]);
    case AST_LIST_PRINT:
      return tac_print_list(ast_node);
    case AST_RET:
      return tac_ret(ast_node);
    case AST_LIST_DECL:
      return tac_join(tac_gen(ast_node->children[0]),
                      tac_gen(ast_node->children[1]));
    case AST_SYMBOL: //FALLTHROUGH
    case AST_ID:
      return tac_new(TAC_SYMBOL, ast_node->symbol, 0, 0);
    case AST_VECT:
      return tac_vec(ast_node);
    case AST_LIT:
      return tac_gen(ast_node->children[0]);
    case AST_CALL:
      return tac_call(ast_node);
    case AST_LIST_ARG:
      return tac_arg_list(ast_node);
  }

  return 0;
}

tac_node_t* tac_new(int type, hash_node_t *res, hash_node_t *op1,
  hash_node_t *op2)
{
  tac_node_t *new_tac = (tac_node_t*) calloc(1, sizeof(tac_node_t));
  new_tac->type = type;
  new_tac->res = res;
  new_tac->op1 = op1;
  new_tac->op2 = op2;
  new_tac->prev = 0;
  return new_tac;
}

tac_node_t* tac_join(tac_node_t *tac1, tac_node_t *tac2)
{
  if(!tac1) return tac2;
  if(!tac2) return tac1;

  tac_node_t *iter = tac2;
  for(iter=tac2; iter->prev != NULL; iter = iter->prev);

  iter->prev = tac1;

  return tac2;
}

void tac_print(tac_node_t *tac_node)
{
  if(tac_node)
  {
    tac_print(tac_node->prev);
    tac_print_node(tac_node);
  }
}

void tac_print_node(tac_node_t *tac_node)
{
  switch (tac_node->type) {
    case TAC_UNDEFINED: printf("TAC_UNDEFINED"); break;
    case TAC_SYMBOL:    printf("TAC_SYMBOL"); break;
    case TAC_MOV:       printf("TAC_MOVE"); break;
    case TAC_TOVECMOV:  printf("TAC_TOVECMOV"); break;
    case TAC_FROMVECMOV:printf("TAC_FROMVECMOV"); break;
    case TAC_ADD:       printf("TAC_ADD"); break;
    case TAC_SUB:       printf("TAC_SUB"); break;
    case TAC_MUL:       printf("TAC_MUL"); break;
    case TAC_DIV:       printf("TAC_DIV"); break;
    case TAC_LESS:      printf("TAC_LESS"); break;
    case TAC_GREATER:   printf("TAC_GREATER"); break;
    case TAC_LE:        printf("TAC_LE"); break;
    case TAC_GE:        printf("TAC_GE"); break;
    case TAC_EQ:        printf("TAC_EQ"); break;
    case TAC_NE:        printf("TAC_NE"); break;
    case TAC_AND:       printf("TAC_AND"); break;
    case TAC_OR:        printf("TAC_OR"); break;
    case TAC_LABEL:     printf("TAC_LABEL"); break;
    case TAC_BEGINFUN:  printf("TAC_BEGINFUN"); break;
    case TAC_ENDFUN:    printf("TAC_ENDFUN"); break;
    case TAC_IFZ:       printf("TAC_IFZ"); break;
    case TAC_JUMP:      printf("TAC_JUMP"); break;
    case TAC_CALL:      printf("TAC_CALL"); break;
    case TAC_ARG:       printf("TAC_ARG"); break;
    case TAC_RET:       printf("TAC_RET"); break;
    case TAC_PRINT:     printf("TAC_PRINT"); break;
    case TAC_READ:      printf("TAC_READ"); break;
    case TAC_PUSHARG:   printf("TAC_PUSHARG"); break;
    case TAC_POPARG:    printf("TAC_POPARG"); break;
    default:            printf("TAC_%d", tac_node->type);
  }
  printf("(");
  if(tac_node->res)
    printf("%s", tac_node->res->text);
  else
    printf(" ");

  if(tac_node->op1)
    printf(", %s", tac_node->op1->text);
  else
    printf(", ");

  if(tac_node->op2)
    printf(", %s)\n", tac_node->op2->text);
  else
    printf(", )\n");
}

hash_node_t* make_temp()
{
    char buffer[60];
    sprintf(buffer, "_temp%d", Temp_Count++);
    return hashInsert(symbolTable, SYMBOL_IDENTIFIER, buffer);
}
