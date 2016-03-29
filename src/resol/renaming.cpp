#include "renaming.hpp"

#include <algorithm>

using namespace std;

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

Renaming::Renaming() :
	next_available_var(1),
	nb_input_variables(0)
{

}

int Renaming::rename_litteral(int l)
{
	unsigned int x = abs(l);

	/* Variable déjà vue, déjà mappée */
	if (variables_mapping.find(x) != variables_mapping.end())
		return sign(l) * variables_mapping[x];

	/* Renommage */
	int mapped_x = variables_mapping[x] = next_available_var++;
	nb_input_variables++;

	return sign(l) * mapped_x;
}

int Renaming::translate_litteral(int l) const
{
	unsigned int x = abs(l);
	unsigned int mapped_x = variables_mapping.at(x);

	return sign(l) * (int) mapped_x;
}

int Renaming::new_variable()
{
	int mapped_x = next_available_var++;

	/* On cherche le prochain nom de variable disponible (input) */
	//!!!! pas efficace, code temporaire
	unsigned int x;
	for (x = 1; variables_mapping.find(x) != variables_mapping.end(); x++);
	variables_mapping[x] = mapped_x;

	return mapped_x;
}

unsigned int Renaming::number_of_input_variables() const
{
	return nb_input_variables;
}

std::map<int, unsigned int>::const_iterator Renaming::begin() const
{
	return variables_mapping.begin();
}

std::map<int, unsigned int>::const_iterator Renaming::end() const
{
	return variables_mapping.end();
}
