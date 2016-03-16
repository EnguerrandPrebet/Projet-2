#ifndef DPLL_SOLVER_UTILS_HPP
#define DPLL_SOLVER_UTILS_HPP

#include "dpll_solver.hpp"

#include <vector>

struct Decision
{
    typedef enum {GUESS, INFER} type;

    type t;

	unsigned int i;
	unsigned int k;

    unsigned int x;
    choice value; // UNKNOWN ne sera pas utilisé
};

choice not_choice(choice x);

choice truth_value(int l, const std::vector<choice>& assignments);

choice satisfy_litteral(int l);

Formula pretreatment(std::vector<choice>& assignments, Formula f, std::ostream& os, int debug);

#endif // DPLL_SOLVER_UTILS_HPP
