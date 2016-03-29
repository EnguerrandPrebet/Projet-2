#include "formula.hpp"
#include "clause.hpp"
#include "prototype.hpp"
#include "formula_io.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <utility>
#include <cmath> // pour l'abs à la base. à virer
#include <cstdlib> // sais pas
#include <map>
#include <algorithm>

#define XSTR(s) STR(s)
#define STR(s) #s

using namespace std;

const static char HELP_OUTPUT[] =
"Utilisation: resol [OPTION]... [FILE(.cnf|.for)]\n"
"\n"
"  -t  --tseitin        use tseitin transform to accept user-friendly .for file\n"
"  -d[N]  --debug       print trace of the SAT resolution\n"
"  -h  --help           print help and exit\n"
"  -wl                  activate watched litterals\n"
"  -rand -moms -dlis    use heuristique\n"
"\n"
"N is an integer between 0 (by default, no output) and " XSTR(MAX_DEBUG) " setting the debug level\n"
"	(notice : debug level is incremental)\n"
"	3: all debug messages printed\n"
"	2: without display during the main algorithm\n"
"	1: without display of the formula before and after the pretreatment\n"
"	0: no debug message printed\n"
"The option --debug sets debug level to maximum\n"
"\n"
"Input file name must ends with .cnf or .for.\n";

int main(int argc, char* argv[])
{
    Option option;
	ostream& os = cout;

    string file_name;
    for(int i = 1; i < argc; i++)
    {
		/* Options en ligne commande */
        if (argv[i][0] == '-')
        {
            string argument(argv[i] + 1);

            if (argument == "-debug")
                option.debug = MAX_DEBUG;

            else if (argument[0] == 'd' && argument.size() == 2) // -d1 à -dMAX_DEBUG
            {
            	option.debug = argument[1] - '0';
                if (option.debug > MAX_DEBUG)
                {
                    cout << "Error: debug level set to inappropriate value: " << option.debug << ", debug level set to MAX_DEBUG" << endl;
                    option.debug = MAX_DEBUG;
                }
            }
            else if (argument == "-tseitin" || argument == "tseitin" || argument == "t")
            {
                option.tseitin = true;

                if (argument == "tseitin")
					cout << "Warning: -tseitin is obsolete, please use --tseitin or -t instead" << endl;
			}
            else if (argument == "-help" || argument == "h")
            {
                cout << HELP_OUTPUT;
				return 0;
			}
			else if (argument == "wl")
			{
				option.watched_litterals = true;
			}
			else if (argument == "cl-interac")
			{
				option.cl_interactive = true;
				option.cl = true;
			}
			else if (argument == "cl")
			{
				option.cl = true;
			}
			else if (argument == "rand")
			{
				option.heuristique = RAND;
			}
			else if (argument == "moms")
			{
				option.heuristique = MOMS;
			}
			else if (argument == "dlis")
			{
				option.heuristique = DLIS;
			}
            else
				cout << "Unrecognized command line option: " << argv[i] << endl;
        }
        else /* A priori un nom de fichier */
        {
            if (string(argv[i]).find(".cnf") != string::npos)
            {
				file_name = argv[i];
                option.cnf_found = true;
			}

            else if (string(argv[i]).find(".for") != string::npos)
            {
				file_name = argv[i];
                option.for_found = true;
				}

            else
            {
                cout << "Unknown command line argument: " << argv[i] << endl;
            }
        }
    }

    if(option.cnf_found == false && option.tseitin == false)
    {
        cout << "Expected input file (.cnf or .for)" << endl;
        return 0;
    }

    ifstream file(file_name, fstream::in);
    if (!file)
    {
		cout << "Cannot open file: " << file_name << endl;
		return 0;
	}

	Formula f;
	unsigned int tseitin_nb_input_variables = 0; // pour éviter le warning

	if (option.cnf_found)
		f = treat_cnf(file, option, os);

	else if (option.tseitin)
		f = treat_tseitin(file_name, tseitin_nb_input_variables, option, cout);

	else
	{
		cout << "Error: no file specified" << endl;
		return 0;
	}

	if(option.cnf_found)
	{
		layered_debug(option, os, "Input read, f is ", 3);
		f.print_formula(os, option,true);

		layered_debug(option, os, "Above, litterals were sorted in a different way in clauses\n", 3);
	}

	layered_debug(option, os, "And now renamed :", 3);
	f.print_formula(os, option, false);

	switch(dpll(f, os, option))
	{
		case TRUE:
			cout << "s SATISFIABLE" << endl;
			f.print_assignment(option, os, (option.tseitin), tseitin_nb_input_variables);
			break;

		case FALSE:
			cout << "s UNSATISFIABLE" << endl;
			break;

		case UNKNOWN:
			cout << "s ???" << endl;
	}

	layered_debug(option, os, "End of main", 1);

	return 0;
}
