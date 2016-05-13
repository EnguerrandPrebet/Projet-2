#ifndef RENAMING_HPP
#define RENAMING_HPP

#include <map>
#include <vector>

#include "../tseitin/truc.hpp"

class Renaming
{
public:
	Renaming();

	int rename_litteral(Real_Value rv);
	int translate_litteral(Real_Value rv) const;

    bool is_input_variable(unsigned int) const;
    Real_Value inverse_translate_litteral(int l) const; // n'appeler que si is_input_variable() == true

	int new_variable();

	unsigned int number_of_variables() const;
	unsigned int number_of_input_variables() const;

	std::map<Real_Value, unsigned int>::const_iterator begin() const;
	std::map<Real_Value, unsigned int>::const_iterator end() const;

private:
	unsigned int next_available_var;
	std::map<Real_Value, unsigned int> variables_mapping;
	std::vector<Real_Value> inverse_variables_mapping;
	unsigned int nb_input_variables;
};

#endif // RENAMING_HPP
