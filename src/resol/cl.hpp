#ifndef CL_HPP
#define CL_HPP

#include "option.hpp"
#include "formula.hpp"

int clause_learning(Formula& f, const stack<Decision_var>& decisions, ostream& os, Option& option);

#endif //CL_HPP
