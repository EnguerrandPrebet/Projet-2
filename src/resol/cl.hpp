#ifndef CL_HPP
#define CL_HPP

#include "option.hpp"
#include "formula.hpp"

int clause_learning(Formula& f, stack<Decision_var>& decisions, ostream& os, Option&);

#endif //CL_HPP
