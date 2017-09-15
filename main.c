
#include <stdio.h>

extern char* yytext;
extern FILE* yyin;
int getLineNumber(void);
int isRunning(void);

int main(int argv, char* argc[])
{
    if(argv < 2)
    {
        printf("./etapa1 in_file\n");
        exit(1);
    }

    yyin = fopen(argc[1], "r");

    while(isRunning())
    {
        int token = yylex();
        printf("line:%d - Found token ", getLineNumber());

        switch (token)
        {
        case KW_BYTE:       printf("KW_BYTE\n"); break;
        case KW_SHORT:      printf("KW_SHORT\n"); break;
        case KW_LONG:       printf("KW_LONG\n"); break;
        case KW_FLOAT:      printf("KW_FLOAT\n"); break;
        case KW_DOUBLE:     printf("KW_DOUBLE\n"); break;
        case KW_THEN:       printf("KW_THEN\n"); break;
        case KW_WHILE:      printf("KW_WHILE\n"); break;
        case KW_FOR:        printf("KW_FOR\n"); break;
        case KW_READ:       printf("KW_READ\n"); break;
        case KW_RETURN:     printf("KW_RETURN\n"); break;
        case KW_PRINT:      printf("KW_PRINT\n"); break;
        case OPERATOR_LE:   printf("OPERATOR_LE\n"); break;
        case OPERATOR_GE:   printf("OPERATOR_GE\n"); break;
        case OPERATOR_EQ:   printf("OPERATOR_EQ\n"); break;
        case OPERATOR_NE:   printf("OPERATOR_NE\n"); break;
        case OPERATOR_AND:  printf("OPERATOR_AND\n"); break;
        case TK_IDENTIFIER: printf("TK_IDENTIFIER, value %s\n", yytext); break;
        case LIT_INTEGER:   printf("LIT_INTEGER, value %s\n", yytext); break;
        case LIT_REAL:      printf("LIT_REAL, value %s\n", yytext); break;
        case LIT_CHAR:      printf("LIT_CHAR, value %s\n", yytext); break;
        case LIT_STRING:    printf("LIT_STRING, value %s\n", yytext); break;
        case TOKEN_ERROR:   printf("TOKEN_ERROR, value %s\n", yytext); break;
        case 0:             printf("End of File\n"); break;
        default:            printf("Special Character, value%s\n", yytext);break;
        }
    }
}