#ifndef CL_HPP
#define CL_HPP

#include "option.hpp"
#include "formula.hpp"

void interface(Option&);
int clause_learning(Formula& f, ostream& os, Option&);
void show_graph();

#endif //CL_HPP
