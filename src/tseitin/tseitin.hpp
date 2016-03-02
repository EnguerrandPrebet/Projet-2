#ifndef TSEITIN_HPP
#define TSEITIN_HPP

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

#include <set>
#include <iostream>

int get_next_var(std::set<int>& var, int& next_var, Formula_input& formula_i);

Formula tseitin(Formula_input& f_input, int debug, std::ostream& os);

#endif // TSEITIN_HPP
