#ifndef PROTOTYPE_HPP
#define PROTOTYPE_HPP

#include "option.hpp"
#include "formula.hpp" // pour state,on peut mieux faire

#include <ostream>

State dpll(Formula&, std::ostream&, Option&);

bool backtrack(Formula&, std::stack<Decision_var>& decisions, std::ostream&, Option&);

void pretreatment(Formula&, std::ostream&, Option& option);

Res update(Formula&, std::stack<Decision_var>& decisions, std::ostream&, Option&);

int get_next_var(Formula& f, std::ostream&, Option&);

#endif // PROTOTYPE_HPP
