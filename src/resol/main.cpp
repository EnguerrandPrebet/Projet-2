#include "formula.hpp"
#include "clause.hpp"
#include "prototype.hpp"
/*
#include "dpll_solver.hpp" //à virer dans un futur proche logiquement
#include "../parser/formula_input.hpp"
#include "../tseitin/tseitin.hpp"
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <utility>
#include <cmath> // pour l'abs à la base. à virer
#include <cstdlib> // sais pas
#include <unistd.h> // dup2 dans treat_tseitin
#include <fcntl.h> // open(2) dans treat_tseitin
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

Formula treat_cnf(istream& is, const Option& option, ostream& os);
//Formula treat_tseitin(string file, int debug, ostream& os);

//Formula_input* parser(int debug, ostream& os);

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

	if (option.cnf_found)
		f = treat_cnf(file, option, os);

	else if (option.tseitin)
        f = Formula(); //treat_tseitin(file_name, option.debug, cout);

	else
	{
		cout << "Error: no file specified" << endl;
		return 0;
	}

	layered_debug(option, os, "Input read, f is ", 3);
	f.print_formula(os, option, INPUT_NAMES);

	layered_debug(option, os, "Above, litterals were sorted in a different way in clauses\n", 3);

	layered_debug(option, os, "And now renamed :", 3);
	f.print_formula(os, option, INTERNAL_NAMES);

	switch(dpll(f, os, option))
	{
		case TRUE:
			cout << "s SATISFIABLE" << endl;
			f.print_assignment(option, os);
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

/***********************************/
/* Cas d'un fichier en forme cnf */
/***********************************/
bool is_commentary(const string& s)
{
	return s[0] == 'c';
}

bool is_end_of_clause(int x)
{
	return x == 0;
}

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

Formula treat_cnf(istream& is, const Option& option, ostream& os)
{
    string line;
    do
    {	getline(is, line);	}
    while (line[0] == 'c');
    istringstream iss(line);

	string p;
    iss >> p;
    if(p != "p")
    {
        cout << "Expected \"p\" on first line (" << p << " given)" << endl;
        exit(0);
    }

    string cnf;
    iss >> cnf;
    if(cnf != "cnf")
    {
        cout << "Expected \"cnf\" after \"p\" (" << cnf << " given)" << endl;
        exit(0);
    }

    unsigned int v,c;
    if (!(iss >> v >> c))
	{
        cout << "Unable to get V and C" << endl;
        exit(0);
	}

    unsigned int actual_v = 0, actual_c = 0;

	list<Clause> clauses({});
	vector<unsigned int> variables_inverse_mapping({0});
	unsigned int next_available_var = 1;
	map<int, unsigned int> variables_mapping({});

	/* On récupère chaque ligne (c'est à dire chaque clause) */
    while (getline(is, line))
    {
		if (is_commentary(line))
            continue;

        istringstream ss(line);
		set<int> clause;
		/* On récupère chaque littéral */
        int x;
        while (ss >> x)
        {
			if (is_end_of_clause(x))
				break;

			/* variable non déjà vue */
			if (variables_mapping.find(abs(x)) == variables_mapping.end())
			{
				variables_inverse_mapping.push_back(abs(x));

				variables_mapping[abs(x)] = next_available_var++;
			}

			/* On ajoute le nouveau littéral à la clause */
			int mapped_x = sign(x) * variables_mapping[abs(x)]; // on remet le signe de x
			clause.insert(mapped_x);

			actual_v = max(actual_v, (unsigned int) abs(x));
        }

		/* On n'ajoute que les clauses bien terminées (terminées par 0) */
        if (x == 0)
        {
            clauses.push_back(list<int>(clause.begin(),clause.end()));
            actual_c++;
        }
		else
		{
			//!!!!!! affichage debug ici
		}
    }

	Formula f(variables_mapping);
	f.set_clauses_alive(clauses);

    if (actual_v != v)
        cout << "Expected " << v << " greatest variable (" << actual_v << " found)" << endl;

    if (actual_c != c)
        cout << "Expected " << c << " clauses (" << actual_c << " found)" << endl;

	if (option.debug >= 1)
        os << "Reading complete !" << endl << "Launching DPLL Solver..." << endl;
	return f;
}

/*
Formula treat_tseitin(string filename, int debug, ostream& os)
{
	int fd = open(filename.c_str(), O_RDONLY);
	dup2(fd, 0);
	Formula f = tseitin(*parser(debug, os), debug, os);

	return f;
}
*/
