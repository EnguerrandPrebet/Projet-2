#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "formula_input.hpp"
#include "std.tab.hpp"
#include "../DPLL/global.hpp"

using namespace std;

extern FILE *stddin;
extern FILE *stddout;
extern int stddparse(void);

extern Formula_input *res_std;

Formula_input* parser_std()
{
	// parse through the input until there is no more:
	do
	{
		stddparse();

		Global::DEBUG(1) << "Post-parser" << endl;
		Global::DEBUG(1) << res_std->to_string() << endl << endl;

	} while (!feof(stddin));

	return res_std;
}

void stdderror(const char *s)
{
	Global::ERROR() << "parse error!  Message: " << s << endl;

	exit(0);
}
