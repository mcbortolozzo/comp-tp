%{
    #include "tokens.h"
    #include "hash.h"

    int lineNumber = 0;
    int running = 1;

    int getLineNumber(void)
    {
        return lineNumber;
    }

    int isRunning(void)
    {
        return running;
    }

%}

DIGIT       [0-9]
LETTER      [a-zA-Z]

%%

    /***** Language Reserved Words *****/

byte                        { return KW_BYTE;       }
short                       { return KW_SHORT;      }
long                        { return KW_LONG;       }
float                       { return KW_FLOAT;      }
double                      { return KW_DOUBLE;     }
when                        { return KW_WHEN;       }
then                        { return KW_THEN;       }
else                        { return KW_ELSE;       }
while                       { return KW_WHILE;      }
for                         { return KW_FOR;        }
read                        { return KW_READ;       }
print                       { return KW_PRINT;      }
return                      { return KW_RETURN;     }

    /***** Misc Characters *****/

\n                          { lineNumber++;         }
(\/\/).*                    /* Comment             */

    /***** Special Characters - TODO check ']' *****/
[-,;:()[\]{}+*/<>=!&$#]     { return yytext[0];     }

    /***** Composed Operators *****/


"<="                        { return OPERATOR_LE;   }
">="                        { return OPERATOR_GE;   }
"=="                        { return OPERATOR_EQ;   }
"!="                        { return OPERATOR_NE;   }
"&&"                        { return OPERATOR_AND;  }
"||"                        { return OPERATOR_OR;   }

    /***** Identifiers  - TODO *****/

    /***** Literals - TODO save literal value *****/

    /* Numbers */

{DIGIT}+                    { return LIT_INTEGER;   }
{DIGIT}+\.{DIGIT}+          { return LIT_REAL;      }

    /* Text - TODO check matching */

'.'                         { return LIT_CHAR;      }
\"(\\.|[^\n\r(\\")])*\"     { return LIT_STRING;    }

" "                         /* Ignore Spaces       */
<<EOF>>                     { running = 0; return 0;}
.                           { return TOKEN_ERROR;   }

%%

int yywrap(void)
{
    return 1;
}


#include "main.c"



