%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "hash.h"

  int running = 1;
  int getLineNumber();
  int yylex();

  void yyerror(const char *str)
  {
    fprintf(stderr, "error on line %d: %s\n", getLineNumber(), str);
    exit(3);
  }

  int isRunning(void)
  {
    return running;
  }

%}

%error-verbose

%union{
  hash_node_t *symbol;
}

%token KW_BYTE KW_SHORT KW_LONG KW_FLOAT KW_DOUBLE
%token KW_IF KW_THEN KW_ELSE KW_WHILE KW_FOR
%token KW_READ KW_RETURN KW_PRINT
%token OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_NE
%token OPERATOR_AND OPERATOR_OR
%token<symbol> TK_IDENTIFIER
%token<symbol> LIT_INTEGER LIT_REAL LIT_CHAR LIT_STRING
%token TOKEN_ERROR

%left OPERATOR_OR
%left OPERATOR_AND
%left OPERATOR_EQ OPERATOR_NE
%left OPERATOR_GE OPERATOR_LE '<' '>'
%left '+' '-'
%left '*' '/'

%start program

%%

program
  : ext_list
  ;

ext_list
  : ext_decl ext_list
  |
  ;

ext_decl
  : ext_var
  | func
  ;

ext_var
  : TK_IDENTIFIER ':' type ext_var_init ';'

type
  : KW_BYTE
  | KW_SHORT
  | KW_LONG
  | KW_FLOAT
  | KW_DOUBLE

ext_var_init
  : '=' lit_var
  | '[' LIT_INTEGER ']' vector_init_list
  ;

vector_init_list
  : lit_var vector_init_list
  |
  ;

lit_var
  : LIT_INTEGER
  | LIT_REAL
  | LIT_CHAR
  ;

func
  : func_header cmd_block
  ;

func_header
  : '(' type ')' TK_IDENTIFIER '(' param_list ')'

param_list
  : param_decl
  | param_list ',' param_decl
  |
  ;

param_decl
  : TK_IDENTIFIER ':' type
  ;

cmd_block
  : '{' cmd_list '}'

cmd_list
  : cmd
  | cmd ';' cmd_list
  ;

cmd
  : attr_cmd
  | flow_control_cmd
  | read_cmd
  | print_cmd
  | return_cmd
  | cmd_block
  |
  ;

attr_cmd
  : TK_IDENTIFIER '=' expr
  | TK_IDENTIFIER '[' expr ']' '=' expr
  ;

read_cmd
  : KW_READ '>' TK_IDENTIFIER
  ;

print_cmd
  : KW_PRINT print_arg_list
  ;

print_arg_list
  : print_arg
  | print_arg_list ',' print_arg
  ;

print_arg
  : expr
  | LIT_STRING
  ;

return_cmd
  : KW_RETURN expr
  ;

flow_control_cmd
  : KW_IF '(' expr ')' KW_THEN cmd
  | KW_IF '(' expr ')' KW_THEN cmd KW_ELSE cmd
  | KW_WHILE '(' expr ')' cmd
  ;

expr
  : expr_var
  | TK_IDENTIFIER '(' call_arg_list ')'
  | expr '+' expr
  | expr '-' expr
  | expr '*' expr
  | expr '/' expr
  | expr '<' expr
  | expr '>' expr
  | expr OPERATOR_LE expr
  | expr OPERATOR_GE expr
  | expr OPERATOR_EQ expr
  | expr OPERATOR_NE expr
  | expr OPERATOR_AND expr
  | expr OPERATOR_OR expr
  | '(' expr ')'
  ;

call_arg_list
  : expr_var
  | call_arg_list ',' expr_var
  ;

expr_var
  : TK_IDENTIFIER
  | TK_IDENTIFIER '[' expr ']'
  | lit_var
  ;

%%

int yywrap()
{
  running = 0;
  return 1;
}
