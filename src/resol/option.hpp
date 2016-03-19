#ifndef OPTION_HPP
#define OPTION_HPP

#include <ostream>
#include <string>

#define MAX_DEBUG 3
#define NO_DEBUG 0

enum Heuristique {NONE, RAND, MOMS, DLIS};

struct Option
{
	bool tseitin = false;

    unsigned int debug = NO_DEBUG;

	bool cnf_found = false;
	bool for_found = false;

    bool watched_litterals = false;

	Heuristique get = NONE;

    bool redirect_os = false;
};

typedef const Option& OptionChoisie;

void layered_debug(Option&, std::ostream& os, const std::string& s, unsigned int X);

#endif // OPTION_HPP
