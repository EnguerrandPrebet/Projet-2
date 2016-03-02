#include "formula.hpp"
#include "dpll_solver.hpp"
#include "../parser/formula_input.hpp"
#include "../tseitin/tseitin.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <utility>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

#define XSTR(s) STR(s)
#define STR(s) #s

using namespace std;

#define MAX_DEBUG 3
#define NO_DEBUG 0

struct Option
{
	bool tseitin = false;
	int debug = NO_DEBUG;
	bool cnf_found = false;
	bool for_found = false;
};

const static char help_output[] =
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

Formula treat_cnf(istream& is, int debug, ostream& os);
Formula treat_tseitin(string file, int debug, ostream& os);
Formula_input* parser(int debug, ostream& os);

int main(int argc, char* argv[])
{
    Option opt;
    string file_name;
    for(int i = 1; i < argc; i++)
    {
		/* Options en ligne commande */
        if (argv[i][0] == '-')
        {
            string argument(argv[i] + 1);

            if (argument == "-debug")
                opt.debug = MAX_DEBUG;

            else if (argument[0] == 'd' && argument.size() == 2) // -d1 à -dMAX_DEBUG
            {
            	opt.debug = argument[1] - '0';
            }
            else if (argument == "-tseitin" || argument == "tseitin" || argument == "t")
            {
                opt.tseitin = true;

                if (argument == "tseitin")
					cout << "Warning: -tseitin is obsolete, please use --tseitin or -t instead" << endl;
			}
            else if (argument == "-help" || argument == "h")
            {
				cout << help_output;
				return 0;
			}

            else
				cout << "Unrecognized command line option: " << argv[i] << endl;
        }
        else /* A priori un nom de fichier */
        {
            if (string(argv[i]).find(".cnf") != string::npos)
            {
				file_name = argv[i];
                opt.cnf_found = true;
			}

            else if (string(argv[i]).find(".for") != string::npos)
            {
				file_name = argv[i];
                opt.for_found = true;
				}

            else
            {
                cout << "Unknown command line argument: " << argv[i] << endl;
            }
        }
    }

    if(opt.cnf_found == false && opt.tseitin == false)
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
	if (opt.cnf_found)
		f = treat_cnf(file, opt.debug, cout);
	else if (opt.tseitin)
		f = treat_tseitin(file_name, opt.debug, cout);
	else
	{
		cout << "Error: no file specified" << endl;
		return 0;
	}

	/* Solving SAT */
	pair< choice, vector<choice> > result = dpll_solver(f, cout, opt.debug);

    if (result.first == TRUE)
    {
        cout << "s SATISFIABLE" << endl;
        for (int i = 1; (unsigned int)i < result.second.size(); i++)
        {
            if (result.second[i] == FALSE)
                cout << -i << " ";
            else if (result.second[i] == UNKNOWN  &&  opt.debug)
                cout << "?" << i << " ";
            else
                cout << i << " ";
        }
        cout << "0" << endl;
    }
    else if (result.first == FALSE)
        cout << "s UNSATISFIABLE" << endl;
    else
        cout << "s ???" << endl;

    return 0;
}

/***********************************/
/* Cas d'un fichier en forme cnf */
/***********************************/
Formula treat_cnf(istream& is, int debug, ostream& os)
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

    Formula f(v);
    set<int> clause;
    /**TODO : à simplifier while cin doit suffire**/
    while (getline(is, line))
    {
        cout << "ligne : " << line << endl;
        if (line[0] == 'c')
            continue;

        istringstream ss(line);
        int x;
        while (ss >> x)
        {
            if (x == 0)
				break;
			clause.insert(x);

            actual_v = max(actual_v, (unsigned int)abs(x));
        }
        if (x == 0)
        {
            f.clauses.push_back(clause);
            clause.clear();
            actual_c++;
        }
    }

    if (actual_v != v)
    {
        cout << "Expected " << v << " greatest variable (" << actual_v << " found)" << endl;
    }

    if (actual_c != c)
    {
        cout << "Expected " << c << " clauses (" << actual_c << " found)" << endl;
    }

    if (debug)
        os << "Reading complete !" << endl << "Launching DPLL Solver..." << endl;

	f.set_v(actual_v);
	return f;
}

Formula treat_tseitin(string filename, int debug, ostream& os)
{
	int fd = open(filename.c_str(), O_RDONLY);
	dup2(fd, 0);
	Formula f = tseitin(*parser(debug, os), debug, os);

	return f;
}
