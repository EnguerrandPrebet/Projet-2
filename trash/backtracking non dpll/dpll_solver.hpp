#ifndef DPLL_SOLVER_HPP
#define DPLL_SOLVER_HPP

#include "formula.hpp"

#include <iostream>
#include <utility>

typedef enum {FALSE, TRUE, UNKNOWN} choice;

std::pair< choice, std::vector<choice> > dpll_solver(Formula f, std::ostream& os, int debug); 

#endif // DPLL_SOLVER_HPP
