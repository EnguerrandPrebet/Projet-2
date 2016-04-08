#include "formula_io.hpp"

#include "../parser/formula_input.hpp"
#include "../tseitin/tseitin.hpp"

#include "renaming.hpp"

#include <sstream>
#include <set>
#include <unistd.h> // dup2 dans treat_tseitin
#include <fcntl.h> // open(2) dans treat_tseitin

using namespace std;

bool is_commentary(const string& s)
{
	return s[0] == 'c';
}

bool is_end_of_clause(const int& x)
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
	Renaming renaming;

	/* On récupère chaque ligne (c'est à dire chaque clause) */
	while (getline(is, line))
	{
		if (is_commentary(line))
			continue;

		istringstream ss(line);
		set<int> clause;
		/* On récupère chaque littéral */
		int l;
		while (ss >> l)
		{
			if (is_end_of_clause(l))
				break;

			/* On ajoute à la clause le littéral renommé */
			int mapped_l = renaming.rename_litteral(l);
			clause.insert(mapped_l);

			actual_v = max(actual_v, (unsigned int) abs(l));
		}

		/* On n'ajoute que les clauses bien terminées (terminées par 0) */
		if (l == 0)
		{
			clauses.push_back(list<int>(clause.begin(), clause.end()));
			actual_c++;
		}
		else
		{
			//!!!!!! affichage debug ici
		}
	}

	Formula f(renaming);
	f.set_clauses_alive(clauses);

	if (actual_v != v)
		cout << "Expected " << v << " greatest variable (" << actual_v << " found)" << endl;

	if (actual_c != c)
		cout << "Expected " << c << " clauses (" << actual_c << " found)" << endl;

	if (option.debug >= 1)
		os << "Reading complete !" << endl << "Launching DPLL Solver..." << endl;

	return f;
}

Formula treat_tseitin(const string& filename, const Option& option, ostream& os)
{
	int fd = open(filename.c_str(), O_RDONLY);
	dup2(fd, 0);
	//!!!!!! traitement des erreurs ici

	Formula f = tseitin(*parser(option, os), option, os);

	//dup2(0,fd); //on remet l'entrée standard (pour cl-interac) (si ça marche)
	close(fd);

	return f;
}
