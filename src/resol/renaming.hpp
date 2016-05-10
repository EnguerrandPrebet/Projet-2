#ifndef RENAMING_HPP
#define RENAMING_HPP

#include <map>
#include <vector>

class Renaming
{
public:
	Renaming();

	int rename_litteral(int l);
	int translate_litteral(int l) const;
	int inverse_translate_litteral(int l) const;

	int new_variable();

	unsigned int number_of_variables() const;
	unsigned int number_of_input_variables() const;

	std::map<int, unsigned int>::const_iterator begin() const;
	std::map<int, unsigned int>::const_iterator end() const;

private:
	unsigned int next_available_var;
	std::map<int, unsigned int> variables_mapping;
	std::vector<unsigned int> inverse_variables_mapping;
	unsigned int nb_input_variables;
};

#endif // RENAMING_HPP
