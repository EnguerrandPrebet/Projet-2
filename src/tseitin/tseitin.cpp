#include "tseitin.hpp"

#include "../parser/formula_input.hpp"
#include "../DPLL/formula.hpp"
#include "../interface/real_value.hpp"

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

Formula tseitin(Formula_input& f_input)
{
	/* Renommage des variables */
	Renaming renaming;
	f_input.compute_renaming(renaming);

	if (Global::option.debug >= 1)
	{
		Global::DEBUG(1) << "Mapping of the input formula variables is: " << endl;
		for (pair<Real_Value, unsigned int> _ : renaming)
		{
			Global::DEBUG(1) << "(" << _.first << " -> " << _.second << ")" << " ";
		}
		Global::DEBUG(1) << endl << endl;
	}

	/* TSEITIN */
	if(f_input.tseitin_x == 0)
		f_input.tseitin_x = renaming.new_variable();

	stack<Formula_input*> jobs;
	jobs.push(&f_input);

	list<Clause> clauses({});

	while(!jobs.empty())
	{
		Formula_input* f_top = jobs.top(); jobs.pop();

		f_top->tseitin_one_step(jobs, clauses, renaming);
	}

	if(renaming.number_of_input_variables() + 1 <= renaming.number_of_variables() - 1)
	Global::DEBUG(1) << "Tseitin added the variables from " << renaming.number_of_input_variables() + 1 << " to " << renaming.number_of_variables() - 1 << " to the mapping !" << endl;

	clauses.push_back(list<int>({f_input.tseitin_x}));

	/* On construit la formule */
	Formula f(renaming);

	f.set_clauses_alive(clauses);

	return f;
}
