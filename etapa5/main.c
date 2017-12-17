#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "hash.h"

extern char* yytext;
extern FILE* yyin;
extern hash_map_t* symbolTable;
int yyparse(void);
int getLineNumber(void);
int isRunning(void);
void initMe(void);

int main(int argv, char* argc[])
{
	initMe();
	if(argv < 2)
    {
        printf("./etapa1 in_file\n");
        exit(1);
    }

    yyin = fopen(argc[1], "r");
		if(!yyin)
		{
			printf("failed to open file\n");
			exit(1);
		}

    int res = yyparse();

    //printf("\nPrinting Symbol Table:\n");
    //hashPrint(symbolTable);

		exit(0);
}
