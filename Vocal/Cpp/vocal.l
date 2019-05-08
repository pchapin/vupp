%{
/****************************************************************************
FILE      : vocal.l
SUBJECT   : Specification for Vocal's lexical analyzer.
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05061
     PChapin@vtc.vsc.edu
****************************************************************************/

#include <cstdlib>
#include <string>
#include "node-types.h"
#include "vocal.tab.hpp"

int current_line = 1;
%}

%%
[ \t\f\r\n] { if (yytext[0] == '\n') current_line++; }
--.*
array       { return ARRAY;      }
begin       { return tBEGIN;     }
else        { return ELSE;       }
end         { return END;        }
function    { return FUNCTION;   }
if          { return IF;         }
int         { return INT;        }
is          { return IS;         }
loop        { return LOOP;       }
long        { return LONG;       }
of          { return OF;         }
return      { return RETURN;     }
returns     { return RETURNS;    }
then        { return THEN;       }
while       { return WHILE;      }
:=          { return ASSIGN;     }

==          { return EQ;         }
!=          { return NE;         }
\<=         { return LE;         }
\>=         { return GE;         }

and         { return AND;        }
or          { return OR;         }
not         { return NOT;        }

[0-9]+[lL]  { // Remove the L first.
              yytext[strlen(yytext) - 1] = '\0'; 
              yylval.numval = std::atoi(yytext); return LNUM; }
[0-9]+      { yylval.numval = std::atoi(yytext); return NUM; }
[a-zA-Z][a-zA-Z0-9_]* {
              std::string *name = new std::string(yytext);
              yylval.stringval = name;
              return IDENTIFIER;
            }
.           { return yytext[0]; }
%%
