#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "formula_input.hpp"
#include "formula_input.tab.hpp"

using namespace std;

extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(void);

extern Formula_input *res;

Formula_input* parser(const Option& option, ostream& os)
{
    // parse through the input until there is no more:
    do
    {
        yyparse();

		if (option.debug >= 1)
		{
			os << "Post-parser: " << endl;
	        os << res->to_string() << endl << endl;
		}
	
    } while (!feof(yyin));

    return res;
}

void yyerror(const char *s)
{
    cout << "EEK, parse error!  Message: " << s << endl;
    // might as well halt now:
    exit(-1);
}

