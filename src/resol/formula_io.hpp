#ifndef FORMULA_IO_HPP
#define FORMULA_IO_HPP

#include "../parser_smt/formula_input.hpp"
#include "formula.hpp"

#include <iostream>

Formula_input* parser_std();
Formula_input* parser_smt();

Formula treat_cnf(std::istream& is);
Formula treat_tseitin(const std::string& file_name);

#endif // FORMULA_IO_HPP
