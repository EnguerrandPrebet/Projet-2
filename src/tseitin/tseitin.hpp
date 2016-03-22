#ifndef TSEITIN_HPP
#define TSEITIN_HPP

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

#include <iostream>

Formula tseitin(Formula_input& f_input, const Option&, std::ostream& os);

#endif // TSEITIN_HPP
