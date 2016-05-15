#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "formula_input.hpp"
#include "smt.tab.hpp"
#include "../resol/global.hpp"

using namespace std;

extern FILE *smtin;
extern FILE *smtout;
extern int smtparse(void);

extern Formula_input *res_smt;

Formula_input* parser_smt()
{
	// parse through the input until there is no more:
	do
	{
		smtparse();

		Global::DEBUG(1) << "Post-parser" << endl;
		Global::DEBUG(1) << res_smt->to_string() << endl << endl;

	} while (!feof(smtin));

	return res_smt;
}

void stdderror(const char *s) // hack : le vrai nom devrait être yyerror, mais on a un autre yyerror dans le projet
{ //! il faudrait s'assurer que c'est bien celui là qui est appelé
	Global::ERROR() << "parse error!  Message: " << s << endl;

	exit(0);
}
