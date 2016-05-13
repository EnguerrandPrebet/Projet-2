%{
#include <iostream>
#include <cstdlib>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "formula_input.tab.hpp"
%}

%%
[ \t]         	;
[1-9][0-9]*	{ yylval.ival = strtoul(yytext,NULL,0); return TK_VAR; }
"="		{ return TK_EQUAL; }
"!="		{ return TK_INEQUAL; }
"("		{ return TK_LPAREN; }
")"		{ return TK_RPAREN; }
"/\\"		{ return TK_AND; }
"\n"		{ return TK_NEWLINE; }
"\\/"		{ return TK_OR; }
"X"		{ return TK_XOR; }
"=>"		{ return TK_IMPLY; }
"<=>"		{ return TK_EQUIV; }
"~"		{ return TK_NEGATE; }
("0\n"|"0")	{ return TK_END; }
.               ;
%%
