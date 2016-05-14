#include "formula_io.hpp"

#include "../parser_smt/formula_input.hpp"
#include "../tseitin/tseitin.hpp"
#include "global.hpp"

#include "renaming.hpp"

#include <sstream>
#include <set>
#include <unistd.h> // dup2 dans treat_tseitin
#include <fcntl.h> // open(2) dans treat_tseitin
#include <cstring> // strerror

using namespace std;

bool is_commentary(const string& s)
{
	return s[0] == 'c';
}

bool is_end_of_clause(int x)
{
	return x == 0;
}

Formula treat_cnf(istream& is)
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
		Global::ERROR() << "Expected \"p\" on first line (" << p << " given)" << endl;
		exit(0);
	}

	string cnf;
	iss >> cnf;
	if(cnf != "cnf")
	{
		Global::ERROR() << "Expected \"cnf\" after \"p\" (" << cnf << " given)" << endl;
		exit(0);
	}

	unsigned int v,c;
	if (!(iss >> v >> c))
	{
		Global::ERROR() << "Unable to get V and C" << endl;
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
		else // ligne terminée sans le 0 indicateur de fin de clause
			Global::WARNING() << "Clause not properly ended" << endl;
	}

	Formula f(renaming);
	f.set_clauses_alive(clauses);

	if (actual_v != v)
		Global::WARNING() << "Expected " << v << " greatest variable (" << actual_v << " found)" << endl;

	if (actual_c != c)
			Global::WARNING() << "Expected " << c << " clauses (" << actual_c << " found)" << endl;

	Global::DEBUG(1) << "Reading complete !" << endl << "Launching DPLL Solver..." << endl;

	return f;
}

Formula treat_tseitin(const string& filename)
{
	errno = 0;
	int fd = open(filename.c_str(), O_RDONLY);

	if (errno != 0)
	{
		Global::ERROR() << strerror(errno) << endl;
		exit(1);
	}

	errno = 0;
	int input_fd = dup(0);
	dup2(fd, 0);
	if (errno != 0)
	{
		Global::ERROR() << strerror(errno) << endl;
		exit(1);
	}

	Formula f;
	if (Global::option.smt)
		f = tseitin(*parser_smt());
	else
		f = tseitin(*parser_std());

	close(fd);

	dup2(input_fd,0);
	return f;
}
