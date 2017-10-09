%{
    #include "hash.h"
    #include "y.tab.h"

    hash_map_t *symbolTable;

    int lineNumber = 1;

    int getLineNumber(void)
    {
        return lineNumber;
    }

    void initMe(void)
    {
      symbolTable = newHashMap(TABLE_SIZE);
    }

%}

DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          [a-zA-Z_](([a-zA-Z_0-9 ]*)[a-zA-Z_0-9])?

%x COMMENT

%%

    /***** Comments *****/

(\/\/).*                    /* Single Line Comnent */

"/*"                        { BEGIN(COMMENT);       }
<COMMENT>"*/"               { BEGIN(INITIAL);       }
<COMMENT>\n                 { lineNumber++;         }
<COMMENT>.                  /* Ignore all          */

    /***** Language Reserved Words *****/

byte                        { return KW_BYTE;       }
short                       { return KW_SHORT;      }
long                        { return KW_LONG;       }
float                       { return KW_FLOAT;      }
double                      { return KW_DOUBLE;     }
if                          { return KW_IF;	    }
then                        { return KW_THEN;       }
else                        { return KW_ELSE;       }
while                       { return KW_WHILE;      }
for                         { return KW_FOR;        }
read                        { return KW_READ;       }
print                       { return KW_PRINT;      }
return                      { return KW_RETURN;     }

    /***** Special Characters *****/
[-,;:()[\]{}+*/<>=!&$#]     { return yytext[0];     }

    /***** Composed Operators *****/


"<="                        { return OPERATOR_LE;   }
">="                        { return OPERATOR_GE;   }
"=="                        { return OPERATOR_EQ;   }
"!="                        { return OPERATOR_NE;   }
"&&"                        { return OPERATOR_AND;  }
"||"                        { return OPERATOR_OR;   }

    /***** Identifiers *****/

{ID}                        {
                              yylval.symbol =
                                hashInsert(symbolTable, TK_IDENTIFIER, yytext);
                              return TK_IDENTIFIER;
                            }

    /***** Literals *****/

        /* Numbers */

{DIGIT}+                    {
                              yylval.symbol =
                                hashInsert(symbolTable, LIT_INTEGER, yytext);
                              return LIT_INTEGER;
                            }
{DIGIT}+\.{DIGIT}+          {
                              yylval.symbol =
                                hashInsert(symbolTable, LIT_REAL, yytext);
                              return LIT_REAL;
                            }

        /* Text */

'.'                         {
                              yylval.symbol =
                                hashInsert(symbolTable, LIT_CHAR, yytext);
                              return LIT_CHAR;
                            }
\"(\\.|[^\n\r\\"])*\"       {
                              yylval.symbol =
                                hashInsert(symbolTable, LIT_STRING, yytext);
                              return LIT_STRING;
                            }

    /***** Misc *****/

\n                          { lineNumber++;         }
[ \t]                       /* Ignore Spaces       */
<<EOF>>                     { return 0;				}
.                           { return TOKEN_ERROR;   }

%%
