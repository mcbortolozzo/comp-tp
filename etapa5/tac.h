#ifndef TAC_H
#define TAC_H

#include "ast.h"
#include "hash.h"
#include "semantic_check.h"

#define TAC_UNDEFINED   1

#define TAC_SYMBOL      126
#define TAC_MOV         127
#define TAC_TOVECMOV    128
#define TAC_FROMVECMOV  129

#define TAC_ADD         130
#define TAC_SUB         131
#define TAC_MUL         132
#define TAC_DIV         133
#define TAC_LESS        134
#define TAC_GREATER     135
#define TAC_LE          136
#define TAC_GE          137
#define TAC_EQ          138
#define TAC_NE          139
#define TAC_AND         140
#define TAC_OR          141
#define TAC_NOT         142

#define TAC_LABEL       145
#define TAC_BEGINFUN    146
#define TAC_ENDFUN      147
#define TAC_IFZ         148
#define TAC_JUMP        149
#define TAC_CALL        150
#define TAC_ARG         151
#define TAC_RET         152
#define TAC_PRINT       153
#define TAC_READ        154

#define TAC_POPARG      161

#define TAC_VARDECL     170
#define TAC_VECTDECL     171
#define TAC_INITVAR     172
#define TAC_INITVECT    173

typedef struct tac_node
{
  int type;
  hash_node_t *res;
  hash_node_t *op1;
  hash_node_t *op2;
  struct tac_node *prev;
} tac_node_t;

hash_map_t *symbolTable;

tac_node_t* tac_gen(ast_node_t *node);
tac_node_t* tac_new(int type, hash_node_t *res, hash_node_t *op1,
  hash_node_t *op2);
tac_node_t* tac_join(tac_node_t *tac1, tac_node_t *tac2);

void tac_print(tac_node_t *tac_node);

#endif
