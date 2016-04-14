#ifndef CL_HPP
#define CL_HPP

#include "formula.hpp"

int clause_learning(Formula& f, const stack<Decision_var>& decisions, Clause& clause_learned);

#endif //CL_HPP
