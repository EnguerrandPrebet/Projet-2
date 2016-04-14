#ifndef FORMULA_IO_HPP
#define FORMULA_IO_HPP

#include "../parser/formula_input.hpp"
#include "formula.hpp"

#include <iostream>

Formula_input* parser();

Formula treat_cnf(std::istream& is);
Formula treat_tseitin(const std::string& file_name);

#endif // FORMULA_IO_HPP
