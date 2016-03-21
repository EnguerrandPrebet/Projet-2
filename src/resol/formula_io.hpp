#ifndef FORMULA_IO_HPP
#define FORMULA_IO_HPP

#include "formula.hpp"
#include "option.hpp"

#include <iostream>

//Formula_input* parser(int debug, ostream& os);

Formula treat_cnf(std::istream& is, const Option& option, std::ostream& os);
Formula treat_tseitin(const std::string& file_name, const Option& option, std::ostream& os);

#endif // FORMULA_IO_HPP
