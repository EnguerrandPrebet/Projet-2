#include "tseitin.hpp"

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

Formula tseitin(Formula_input& f_input, unsigned int& nb_input_variables, const Option& option, ostream& os)
{
	if (option.debug >= 1)
	{
		os << "Mapping of the input formula variables is: " << endl;
		for (pair<int, unsigned int> _ : f_input.variables_mapping)
		{

			os << "(" << _.first << " -> " << _.second << ")" << " ";
		}
		os << endl << endl;
	}

	/* TSEITIN */
	f_input.tseitin_x = Formula_input::new_variable();

	stack<Formula_input*> jobs;
	jobs.push(&f_input);

	list<Clause> clauses({});

	while(!jobs.empty())
	{
		Formula_input* f_top = jobs.top(); jobs.pop();

		f_top->tseitin_one_step(jobs, clauses);
	}


	if (option.debug >= 1)
	{
		os << "Tseitin added the following variables to the mapping : " << endl;
		for (pair<int, unsigned int> _ : f_input.variables_mapping)
		{
			//if (_.second >= Formula_input::nb_input_variables)
				os << "(" << _.first << " -> " << _.second << ")" << " ";
		}
		os << endl << endl;
	}

	clauses.push_back(list<int>({f_input.tseitin_x}));
	/* On construit la formule */
	Formula f(Formula_input::variables_mapping,Formula_input::next_available_var);
	f.set_clauses_alive(clauses);

	return f;
}
