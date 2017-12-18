#include "asm.h"

hash_map_t *symbolTable;

void gen_vars(tac_node_t *tac, hash_map_t *symbolTable);
void gen_temps(hash_map_t *map);
void gen_temp(hash_node_t *symbol);
void gen_string(hash_node_t *symbol);
void gen_float(hash_node_t *symbol);
void gen_var(tac_node_t *tac);

void gen_code(tac_node_t* tac);
void print_asm(tac_node_t* tac_print);
void read_asm(tac_node_t* node);
void op_asm(tac_node_t* node);
void mov_asm(tac_node_t* node);
void tovec_asm(tac_node_t *node);
void fromvec_asm(tac_node_t *node);
void cmp_asm(tac_node_t *node, int is_float);
void jz_asm(tac_node_t *node);

int strCount = 0;
int doubleCount = 0;
int cmpCount = 0;

void generate_asm(tac_node_t *tac)
{
  gen_vars(tac, symbolTable);

  printf("\n\n.section .text\n");
  printf("\t .global main\n");
  printf("main:\n");
  gen_code(tac);
}

void gen_vars(tac_node_t *tac, hash_map_t *symbolTable)
{
  printf("\t.data\n");
  printf(".formatint:\n\t.asciz \"%%d\"\n");
  printf(".formatdouble:\n\t.asciz \"%%lf\"\n");
  printf(".formatstring:\n\t.asciz \"%s\"\n", "%s");
  printf(".formatchar:\n\t.asciz \"%%c\"\n");

  gen_temps(symbolTable);
  gen_var(tac);
}

void gen_temps(hash_map_t *map)
{
  int i;
  for(i = 0; i < TABLE_SIZE; i++)
  {
    hash_node_t *curr =  map->nodes[i];
    while(curr != NULL)
    {
      gen_temp(curr);
      gen_string(curr);
      gen_float(curr);
      curr = curr->next;
    }
  }
}

void gen_temp(hash_node_t *symbol)
{
  if(symbol->type != SYMBOL_TEMP) return;

  printf("\t.comm %s,8\n", symbol->text);
}

void gen_float(hash_node_t *symbol)
{
  if(symbol->type != SYMBOL_LIT_REAL) return;

  printf("_double_%d:\n\t.double %s\n", doubleCount, symbol->text);
  symbol->nature = doubleCount++;
}

void gen_string(hash_node_t *symbol)
{
  if(symbol->type != SYMBOL_LIT_STRING) return;

  printf("_str_%d:\n\t.string %s\n", strCount, symbol->text);
  symbol->dataType = strCount++;
}

void gen_var(tac_node_t *tac)
{
  if(!tac) return;

  gen_var(tac->prev);
  switch (tac->type) {
    case TAC_VARDECL:     printf("%s:\n", tac->res->text); break;
    case TAC_INITVAR:
    if(tac->op1->dataType == TYPE_INT)
      printf("\t.quad %s\n", tac->op1->text);
    else
      printf("\t.double %s\n", tac->op1->text);
    break;
    case TAC_VECTDECL:
    if(tac->res->dataType == TYPE_INT)
      printf("%s:\n\t.quad ", tac->res->text);
    else
      printf("%s:\n\t.double ", tac->res->text);
    break;
    case TAC_INITVECT:
      if(tac->prev->type != TAC_VECTDECL)
      printf(",");
      printf("%s", tac->op2->text); break;
  }
}

void gen_code(tac_node_t* tac)
{
  if(!tac) return;
  gen_code(tac->prev);
  switch (tac->type) {
    case TAC_MOV:         mov_asm(tac); break;
    case TAC_TOVECMOV:    tovec_asm(tac); break;
    case TAC_FROMVECMOV:  fromvec_asm(tac); break;
    case TAC_NE:
    case TAC_LESS:
    case TAC_LE:
    case TAC_GREATER:
    case TAC_GE:
    case TAC_EQ:
    case TAC_SUB:
    case TAC_MUL:
    case TAC_DIV:
    case TAC_ADD:         op_asm(tac); break;
    case TAC_PRINT:       print_asm(tac); break;
    case TAC_READ:        read_asm(tac); break;
    case TAC_ENDFUN:      printf("ret\n"); break;
    case TAC_IFZ:         jz_asm(tac); break;
    case TAC_JUMP:        printf("\tjmp %s\n", tac->op1->text); break;
    case TAC_LABEL:       printf("%s:\n", tac->op1->text); break;
  }
}

int is_var_type(hash_node_t *node)
{
  return node->type == SYMBOL_IDENTIFIER
          || node->type == SYMBOL_TEMP;
}

void mov_asm(tac_node_t* node)
{
  if(is_var_type(node->op1))
  {
    if(node->op1->dataType == TYPE_FLOAT)
    {
      printf("\tmovsd %s, %%xmm1\n", node->op1->text);
      printf("\tmovsd %%xmm1, %s\n", node->res->text);
    }
    else
    {
      printf("\tmovq %s, %%rax\n", node->op1->text);
      printf("\tmovq %%rax, %s\n", node->res->text);
    }
  }
  else
  {
    if(node->op1->dataType == TYPE_FLOAT)
    {
      printf("\tmovsd _double_%d, %%xmm1\n", node->op1->nature);
      printf("\tmovsd %%xmm1, %s\n", node->res->text);
    }
    else
    {
      printf("\tmovq $%s, %%rax\n", node->op1->text);
      printf("\tmovq %%rax, %s\n", node->res->text);
    }
  }
}

void tovec_asm(tac_node_t *node)
{

  if(is_var_type(node->op1))
    printf("\tmovq %s, %%rbx\n", node->op1->text);
  else
    printf("\tmovq $%s, %%rbx\n", node->op1->text);

  if(is_var_type(node->op2))
  {
    if(node->op1->dataType == TYPE_FLOAT)
    {
      printf("\tmovsd %s, %%xmm1\n", node->op2->text);
      printf("\tmovsd %%xmm1, %s(,%%rbx,8)\n", node->res->text);
    }
    else
    {
      printf("\tmovq %s, %%rax\n", node->op2->text);
      printf("\tmovq %%rax, %s(,%%rbx,8)\n", node->res->text);
    }
  }
  else
  {
    if(node->op2->dataType == TYPE_FLOAT)
    {
      printf("\tmovsd _double_%d, %%xmm1\n", node->op2->nature);
      printf("\tmovsd %%xmm1, %s(,%%rbx,8)\n", node->res->text);
    }
    else
    {
      printf("\tmovq $%s, %%rax\n", node->op2->text);
      printf("\tmovq %%rax, %s(,%%rbx,8)\n", node->res->text);
    }
  }
}

void fromvec_asm(tac_node_t *node)
{
  if(is_var_type(node->op1))
    printf("\tmovq %s, %%rbx\n", node->op1->text);
  else
    printf("\tmovq $%s, %%rbx\n", node->op1->text);


  if(node->op2->dataType == TYPE_FLOAT)
  {
    printf("\tmovsd %s(,%%rbx,8), %%xmm1\n", node->op2->text);
    printf("\tmovsd %%xmm1, %s\n", node->res->text);
  }
  else
  {
    printf("\tmovq %s(,%%rbx,8), %%rax\n", node->op2->text);
    printf("\tmovq %%rax, %s\n", node->res->text);
  }
}

void op_asm(tac_node_t* node)
{
  if(node->op1->dataType == TYPE_FLOAT || node->op2->dataType == TYPE_FLOAT)
  {
    if(is_var_type(node->op1))
    {
      if(node->op1->dataType == TYPE_FLOAT)
        printf("\tmovsd %s, %%xmm1\n", node->op1->text);
      else
      {
        printf("\tmovq %s, %%rax\n", node->op1->text);
        printf("\tcvtsi2sdq %%rax, %%xmm1\n");
      }
    }
    else
    {
      if(node->op1->type != SYMBOL_LIT_REAL)
      {
        printf("\tmovq $%s, %%rax\n", node->op1->text);
        printf("\tcvtsi2sdq %%rax, %%xmm1\n");
      }
      else
      {
        printf("\tmovsd _double_%d, %%xmm1\n", node->op1->nature);
      }
    }

    if(is_var_type(node->op2))
    {
      if(node->op2->dataType == TYPE_FLOAT)
        printf("\tmovsd %s, %%xmm0\n", node->op2->text);
      else
      {
        printf("\tmovq %s, %%rbx\n", node->op2->text);
        printf("\tcvtsi2sdq %%rax, %%xmm0\n");
      }
    }
    else
    {
      if(node->op2->type != SYMBOL_LIT_REAL)
      {
        printf("\tmovq $%s, %%rbx\n", node->op2->text);
        printf("\tcvtsi2sdq %%rbx, %%xmm0\n");
      }
      else
      {
        printf("\tmovsd _double_%d, %%xmm0\n", node->op2->nature);
      }
    }
    switch (node->type) {
      case TAC_ADD: printf("\taddsd %%xmm0, %%xmm1\n"); break;
      case TAC_SUB: printf("\tsubsd %%xmm0, %%xmm1\n"); break;
      case TAC_MUL: printf("\tmulsd %%xmm0, %%xmm1\n"); break;
      case TAC_DIV: printf("\tdivsd %%xmm0, %%xmm1\n"); break;
      case TAC_NE:
      case TAC_LESS:
      case TAC_LE:
      case TAC_GREATER:
      case TAC_GE:
      case TAC_EQ: cmp_asm(node, 1); return;
    }

    printf("\tmovsd %%xmm1, %s\n", node->res->text);
  }
  else
  {
    if(is_var_type(node->op1))
      printf("\tmovq %s, %%rax\n", node->op1->text);
    else
      printf("\tmovq $%s, %%rax\n", node->op1->text);
    if(is_var_type(node->op2))
      printf("\tmovq %s, %%rbx\n", node->op2->text);
    else
      printf("\tmovq $%s, %%rbx\n", node->op2->text);


    switch (node->type) {
      case TAC_ADD: printf("\taddq %%rbx, %%rax\n"); break;
      case TAC_SUB: printf("\tsubq %%rbx, %%rax\n"); break;
      case TAC_MUL: printf("\timulq %%rbx, %%rax\n"); break;
      case TAC_DIV: printf("\tcqto\n\tidivq %%rbx\n"); break;
      case TAC_NE:
      case TAC_LESS:
      case TAC_LE:
      case TAC_GREATER:
      case TAC_GE:
      case TAC_EQ: cmp_asm(node, 0); return;
    }

    printf("\tmovq %%rax, %s\n", node->res->text);
  }
}

void cmp_asm(tac_node_t *node, int is_float)
{
  if(is_float)
  {
    printf("\tucomisd %%xmm0, %%xmm1\n");
    printf("\tjp _cmp_f_%d\n", cmpCount);
    switch (node->type) {
      case TAC_EQ:      printf("\tjne _cmp_f_%d\n", cmpCount); break;
      case TAC_NE:      printf("\tje _cmp_f_%d\n", cmpCount); break;
      case TAC_LESS:    printf("\tjae _cmp_f_%d\n", cmpCount); break;
      case TAC_LE:      printf("\tja _cmp_f_%d\n", cmpCount); break;
      case TAC_GREATER: printf("\tjbe _cmp_f_%d\n", cmpCount); break;
      case TAC_GE:      printf("\tjb _cmp_f_%d\n", cmpCount); break;
    }
  }
  else
  {
    printf("\tcmp %%rbx, %%rax\n");
    switch (node->type) {
      case TAC_EQ:      printf("\tjne _cmp_f_%d\n", cmpCount); break;
      case TAC_NE:      printf("\tje _cmp_f_%d\n", cmpCount); break;
      case TAC_LESS:    printf("\tjge _cmp_f_%d\n", cmpCount); break;
      case TAC_LE:      printf("\tjg _cmp_f_%d\n", cmpCount); break;
      case TAC_GREATER: printf("\tjle _cmp_f_%d\n", cmpCount); break;
      case TAC_GE:      printf("\tjl _cmp_f_%d\n", cmpCount); break;
    }
  }

  printf("\tmovq $1, %%rax\n");
  printf("\tjmp _cmp_s_%d\n", cmpCount);
  printf("_cmp_f_%d:\n", cmpCount);
  printf("\tmovq $0, %%rax\n");
  printf("_cmp_s_%d:\n", cmpCount);
  printf("\tmovq %%rax, %s\n", node->res->text);
  cmpCount++;
}

void print_asm(tac_node_t *node)
{
  printf("\tpushq %%rbp\n");
  printf("\tmovq %%rsp, %%rbp\n");
  switch (node->op1->type) {
    case SYMBOL_LIT_INT:
      printf("\tmovl $%s, %%esi\n", node->op1->text);
      printf("\tmovl $.formatint, %%edi\n");
      printf("\tmovl $0, %%eax\n");
      break;
    case SYMBOL_LIT_CHAR:
      printf("\tmovq $%s, %%rdi\n", node->op1->text);
      printf("\tcall putchar\n");
      printf("\tpopq %%rbp\n");
      return;
    case SYMBOL_LIT_REAL:
      printf("\tmovsd _double_%d, %%xmm0\n", node->op1->nature);
      printf("\tmovl $.formatdouble, %%edi\n");
      printf("\tmovl $1, %%eax\n");
      break;
    case SYMBOL_IDENTIFIER:
    case SYMBOL_TEMP:
      if(node->op1->dataType == TYPE_INT)
      {
        printf("\tmovl %s, %%esi\n", node->op1->text);
        printf("\tmovl $.formatint, %%edi\n");
        printf("\tmovl $0, %%eax\n");
      }
      else if(node->op1->dataType == TYPE_FLOAT)
      {
        printf("\tmovsd %s, %%xmm0\n", node->op1->text);
        printf("\tmovl $.formatdouble, %%edi\n");
        printf("\tmovl $1, %%eax\n");
      }
      break;
    case SYMBOL_LIT_STRING:
      printf("\tmovl $_str_%d, %%esi\n", node->op1->dataType);
      printf("\tmovl $.formatstring, %%edi\n");
      printf("\tmovl $0, %%eax\n");
      break;
  }
  printf("\tcall printf\n");
  printf("\tpopq %%rbp\n");
}

void read_asm(tac_node_t *node)
{
    if(node->res->dataType == TYPE_INT)
    {
      printf("\tmovq $%s, %%rsi\n", node->res->text);
      printf("\tmovq $.formatint, %%rdi\n");
      printf("\tmovq $0, %%rax\n");
      printf("\tcall scanf\n");
    }
    else if(node->res->dataType == TYPE_FLOAT)
    {
      printf("\tmovq $%s, %%rsi\n", node->res->text);
      printf("\tmovq $.formatdouble, %%rdi\n");
      printf("\tmovq $0, %%rax\n");
      printf("\tcall scanf\n");
    }
}

void jz_asm(tac_node_t *node)
{
  printf("\tmovq %s, %%rax\n", node->op1->text);
  printf("\tcmp $0, %%rax\n");
  printf("\tje %s\n", node->op2->text);
}
