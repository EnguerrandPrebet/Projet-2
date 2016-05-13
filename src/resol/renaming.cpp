#include "renaming.hpp"

#include <algorithm>

using namespace std;

int sign(const int& val) {
	return (0 < val) - (val < 0);
}

Renaming::Renaming() :
	next_available_var(1),
	nb_input_variables(0)
{
	inverse_variables_mapping.push_back(0); // 0 -> 0, inverse_variables_mapping[0] est inutilisé
}

int Renaming::rename_litteral(Real_Value rv)
{
	Real_Value x = abs(rv);

	/* Variable déjà vue, déjà mappée */
	if (variables_mapping.find(x) != variables_mapping.end())
		return sign(rv) * variables_mapping[x];

	/* Renommage */
	int mapped_x = variables_mapping[x] = next_available_var++;
	inverse_variables_mapping.push_back(x); // next_available_var -> x
	nb_input_variables++;

	return sign(rv) * mapped_x;
}

int Renaming::translate_litteral(Real_Value rv) const
{
	Real_Value x = abs(rv);
	unsigned int mapped_x = variables_mapping.at(x);

	return sign(rv) * (int) mapped_x;
}

bool Renaming::is_input_variable(unsigned int x) const
{
	return x <= nb_input_variables;
}

Real_Value Renaming::inverse_translate_litteral(int l) const
{
	unsigned int x = abs(l);
	Real_Value mapped_x = inverse_variables_mapping[x];

	return sign(l) * mapped_x;
}

int Renaming::new_variable()
{
	int mapped_x = next_available_var++;

	return mapped_x;
}

unsigned int Renaming::number_of_variables() const
{
	return next_available_var;
}

unsigned int Renaming::number_of_input_variables() const
{
	return nb_input_variables;
}

std::map<Real_Value, unsigned int>::const_iterator Renaming::begin() const
{
	return variables_mapping.begin();
}

std::map<Real_Value, unsigned int>::const_iterator Renaming::end() const
{
	return variables_mapping.end();
}
