#ifndef PROTOTYPE_HPP
#define PROTOTYPE_HPP

#include "formula.hpp" // pour state,on peut mieux faire

State dpll(Formula&);

bool backtrack(Formula&, std::stack<Decision>& decisions);

void pretreatment(Formula&);

Res update(Formula&, std::stack<Decision>& decisions);

int get_next_assignment(const Formula& f);

#endif // PROTOTYPE_HPP
