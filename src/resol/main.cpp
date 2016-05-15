#include "formula.hpp"
#include "clause.hpp"
#include "dpll.hpp"
#include "formula_io.hpp"
#include "global.hpp"

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

string retrieve_cmd_arguments(int argc, char* argv[]); // retourne le nom du fichier à ouvrir

int main(int argc, char* argv[])
{
	string file_name = retrieve_cmd_arguments(argc, argv);

	if(Global::option.cnf_found == false && Global::option.tseitin == false)
	{
		Global::ERROR() << "expected input file (.cnf or .for)" << endl;
		return 0;
	}

	ifstream file(file_name, fstream::in);
	if (!file)
	{
		Global::ERROR() << "cannot open file: " << file_name << endl;
		return 1;
	}

	Formula f;
	if (Global::option.cnf_found)
		f = treat_cnf(file);

	else if (Global::option.tseitin)
		f = treat_tseitin(file_name);

	else
	{
		Global::ERROR() << "no file specified" << endl;
		return 0;
	}

	if(Global::option.cnf_found)
	{
		Global::DEBUG(3) << "Input read, f is ";

		if (Global::option.debug >= 3)
			f.print_formula(true);

		Global::DEBUG(3) << "Above, litterals were sorted in a different way in clauses" << endl;
	}

	Global::DEBUG(3) << "And now renamed :";

	if (Global::option.debug >= 3)
		f.print_formula(false);

	switch (dpll(f))
	{
		case TRUE:
			Global::MSG() << "s SATISFIABLE" << endl;
			f.print_assignment();
			break;

		case FALSE:
			Global::MSG() << "s UNSATISFIABLE" << endl;
			break;

		case UNKNOWN:
			Global::MSG() << "s ???" << endl;
	}


	Global::DEBUG(1) << "End of main" << endl;

	return 0;
}

string retrieve_cmd_arguments(int argc, char* argv[])
{
	string file_name;
	for(int i = 1; i < argc; i++)
	{
		/* Options en ligne commande */
		if (argv[i][0] == '-')
		{
			string argument(argv[i] + 1);

			if (argument == "-debug")
			{
				Global::option.debug = MAX_DEBUG;
			}

			else if (argument[0] == 'd' && argument.size() == 2) // -d1 à -dMAX_DEBUG
			{
				Global::option.debug = argument[1] - '0';
				if (Global::option.debug > MAX_DEBUG)
				{
					Global::ERROR() << "debug level set to inappropriate value: " << Global::option.debug << ", debug level set to MAX_DEBUG" << endl;
					Global::option.debug = MAX_DEBUG;
				}
			}
			else if (argument == "-tseitin" || argument == "tseitin" || argument == "t")
			{
				Global::option.tseitin = true;
				if (argument == "tseitin")
					Global::WARNING() << "-tseitin is obsolete, please use --tseitin or -t instead" << endl;
			}
			else if (argument == "smt")
			{
				Global::option.smt = true;
				Global::option.tseitin = true;
			}
			else if (argument == "-help" || argument == "h")
			{
				Global::MSG() << HELP_OUTPUT;
				exit(0);
			}
			else if (argument == "wl")
			{
				Global::option.watched_litterals = true;
			}
			else if (argument == "cl-interac")
			{
				Global::option.cl_interactive = true;
				Global::option.cl = true;
			}
			else if (argument == "cl")
			{
				Global::option.cl = true;
			}
			else if (argument == "rand")
			{
				Global::option.heuristique = RAND;
			}
			else if (argument == "moms")
			{
				Global::option.heuristique = MOMS;
			}
			else if (argument == "dlis")
			{
				Global::option.heuristique = DLIS;
			}
			else
				Global::WARNING() << "unrecognized command line option: " << argv[i] << endl;
		}
		else /* A priori un nom de fichier */
		{
			if (string(argv[i]).find(".cnf") != string::npos)
			{
				file_name = argv[i];
				Global::option.cnf_found = true;
			}

			else if (string(argv[i]).find(".for") != string::npos)
			{
				file_name = argv[i];
				Global::option.for_found = true;
				}

			else
			{
				Global::WARNING() << "unknown command line argument: " << argv[i] << endl;
			}
		}
	}

	return file_name;
}
