%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "hash.h"
  #include "ast.h"
  #include "tac.h"
  #include "semantic_check.h"

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
  ast_node_t *node;
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
%right '!'

%start program

%type<node> program
%type<node> ext_list
%type<node> ext_decl
%type<node> ext_var
%type<node> type
%type<node> ext_var_init
%type<node> vector_init_list
%type<node> func
%type<node> func_header
%type<node> param_list
%type<node> param_decl
%type<node> cmd_block
%type<node> cmd_list
%type<node> cmd
%type<node> attr_cmd
%type<node> read_cmd
%type<node> print_cmd
%type<node> print_arg_list
%type<node> print_arg
%type<node> return_cmd
%type<node> flow_control_cmd
%type<node> expr
%type<node> call_arg_list
%type<node> lit_var

%%

program
  : ext_list { check_tree($1); tac_print(tac_gen($1));}
  ;

ext_list
  : ext_decl ext_list { $$ = ast_create(AST_LIST_DECL, 0, $1, $2, 0, 0); }
  | { $$ = 0; }
  ;

ext_decl
  : ext_var
  | func
  ;

ext_var
  : TK_IDENTIFIER ':' type ext_var_init ';' { $$ = ast_create(AST_VAR_DECL, $1, $3, $4, 0, 0); }

type
  : KW_BYTE { $$ = ast_create(AST_BYTE, 0, 0, 0, 0, 0); }
  | KW_SHORT { $$ = ast_create(AST_SHORT, 0, 0, 0, 0, 0); }
  | KW_LONG { $$ = ast_create(AST_LONG, 0, 0, 0, 0, 0); }
  | KW_FLOAT { $$ = ast_create(AST_FLOAT, 0, 0, 0, 0, 0); }
  | KW_DOUBLE { $$ = ast_create(AST_DOUBLE, 0, 0, 0, 0, 0); }

ext_var_init
  : '=' lit_var { $$ = ast_create(AST_VAR_INIT, 0, $2, 0, 0, 0); }
  | '[' LIT_INTEGER ']' vector_init_list { $$ = ast_create(AST_VECT_INIT, $2, $4, 0, 0, 0); }
  ;

vector_init_list
  : vector_init_list lit_var { $$ = ast_create(AST_LIST_VECT_INIT, 0, $2, $1, 0, 0); }
  | { $$ = 0; }
  ;

lit_var
  : LIT_INTEGER { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
  | LIT_REAL { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
  | LIT_CHAR { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
  ;

func
  : func_header cmd_block { $$ = ast_create(AST_FUNC, 0, $1, $2, 0, 0); }
  ;

func_header
  : '(' type ')' TK_IDENTIFIER '(' param_list ')'
      { $$ = ast_create(AST_FUNC_HEADER, $4, $2, $6, 0, 0); }

param_list
  : param_decl { $$ = ast_create(AST_LIST_PARAM_DECL, 0, $1, 0, 0, 0); }
  | param_list ',' param_decl { $$ = ast_create(AST_LIST_PARAM_DECL, 0, $3, $1, 0, 0); }
  | { $$ = 0; }
  ;

param_decl
  : TK_IDENTIFIER ':' type { $$ = ast_create(AST_PARAM_DECL, $1, $3, 0, 0, 0); }
  ;

cmd_block
  : '{' cmd_list '}' { $$ = ast_create(AST_CMD_BLOCK, 0, $2, 0, 0, 0); }

cmd_list
  : cmd { $$ = ast_create(AST_CMD_LIST, 0, $1, 0, 0, 0); }
  | cmd ';' cmd_list { $$ = ast_create(AST_CMD_LIST, 0, $1, $3, 0, 0); }
  ;

cmd
  : attr_cmd
  | flow_control_cmd
  | read_cmd
  | print_cmd
  | return_cmd
  | cmd_block
  | { $$ = 0;}
  ;

attr_cmd
  : TK_IDENTIFIER '=' expr  { $$ = ast_create(AST_ATTR, $1, $3, 0, 0, 0); }
  | TK_IDENTIFIER '[' expr ']' '=' expr { $$ = ast_create(AST_ATTR, $1, $6, $3, 0, 0); }
  ;

read_cmd
  : KW_READ '>' TK_IDENTIFIER { $$ = ast_create(AST_READ, $3, 0, 0, 0, 0); }
  ;

print_cmd
  : KW_PRINT print_arg_list { $$ = ast_create(AST_PRINT, 0, $2, 0, 0, 0); }
  ;

print_arg_list
  : print_arg { $$ = ast_create(AST_LIST_PRINT, 0, $1, 0, 0, 0); }
  | print_arg_list ',' print_arg { $$ = ast_create(AST_LIST_PRINT, 0, $3, $1, 0, 0); }
  ;

print_arg
  : expr
  | LIT_STRING { $$ = ast_create(AST_SYMBOL, $1, 0, 0, 0, 0); }
  ;

return_cmd
  : KW_RETURN expr { $$ = ast_create(AST_RET, 0, $2, 0, 0, 0); }
  ;

flow_control_cmd
  : KW_IF '(' expr ')' KW_THEN cmd  { $$ = ast_create(AST_IF, 0, $3, $6, 0, 0); }
  | KW_IF '(' expr ')' KW_THEN cmd KW_ELSE cmd { $$ = ast_create(AST_IF, 0, $3, $6, $8, 0); }
  | KW_WHILE '(' expr ')' cmd { $$ = ast_create(AST_WHILE, 0, $3, $5, 0, 0); }
  ;

expr
  : TK_IDENTIFIER { $$ = ast_create(AST_ID, $1, 0, 0, 0, 0); }
  | TK_IDENTIFIER '[' expr ']' { $$ = ast_create(AST_VECT, $1, $3, 0, 0, 0); }
  | lit_var {$$ = ast_create(AST_LIT, 0, $1, 0, 0, 0); }
  | TK_IDENTIFIER '(' call_arg_list ')' { $$ = ast_create(AST_CALL, $1, $3, 0, 0, 0); }
  | '!' expr { $$ = ast_create(AST_NOT, 0, $2, 0, 0, 0); }
  | expr '+' expr { $$ = ast_create(AST_ADD, 0, $1, $3, 0, 0); }
  | expr '-' expr { $$ = ast_create(AST_SUB, 0, $1, $3, 0, 0); }
  | expr '*' expr { $$ = ast_create(AST_MUL, 0, $1, $3, 0, 0); }
  | expr '/' expr { $$ = ast_create(AST_DIV, 0, $1, $3, 0, 0); }
  | expr '<' expr { $$ = ast_create(AST_LESS, 0, $1, $3, 0, 0); }
  | expr '>' expr { $$ = ast_create(AST_GREATER, 0, $1, $3, 0, 0); }
  | expr OPERATOR_LE expr { $$ = ast_create(AST_LE, 0, $1, $3, 0, 0); }
  | expr OPERATOR_GE expr { $$ = ast_create(AST_GE, 0, $1, $3, 0, 0); }
  | expr OPERATOR_EQ expr { $$ = ast_create(AST_EQ, 0, $1, $3, 0, 0); }
  | expr OPERATOR_NE expr { $$ = ast_create(AST_NE, 0, $1, $3, 0, 0); }
  | expr OPERATOR_AND expr { $$ = ast_create(AST_AND, 0, $1, $3, 0, 0); }
  | expr OPERATOR_OR expr { $$ = ast_create(AST_OR, 0, $1, $3, 0, 0); }
  | '(' expr ')' { $$ = $2; }
  ;

call_arg_list
  : expr { $$ = ast_create(AST_LIST_ARG, 0, $1, 0, 0, 0); }
  | call_arg_list ',' expr { $$ = ast_create(AST_LIST_ARG, 0, $3, $1, 0, 0); }
  ;

%%

int yywrap()
{
  running = 0;
  return 1;
}
