#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "formula_input.hpp"
#include "formula_input.tab.hpp"
#include "../resol/global.hpp"

using namespace std;

extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(void);

extern Formula_input *res;

Formula_input* parser()
{
	// parse through the input until there is no more:
	do
	{
		yyparse();

		Global::DEBUG(1) << "Post-parser" << endl;
		Global::DEBUG(1) << res->to_string() << endl << endl;

	} while (!feof(yyin));

	return res;
}

void yyerror(const char *s)
{
	Global::ERROR() << "parse error!  Message: " << s << endl;

	exit(0);
}

