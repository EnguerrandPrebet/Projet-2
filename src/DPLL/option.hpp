#ifndef OPTION_HPP
#define OPTION_HPP

#include <ostream>
#include <string>

#define MAX_DEBUG 3
#define NO_DEBUG 0

enum Heuristique {NONE, RAND, MOMS, DLIS, VSIDS};

struct Option
{
	bool tseitin = false;
	bool smt = false;

	unsigned int debug = NO_DEBUG;

	bool cnf_found = false;
	bool for_found = false;

	bool watched_litterals = false;

	Heuristique heuristique = NONE;

	bool cl_interactive = false;
	bool cl = false;
};

#endif // OPTION_HPP
