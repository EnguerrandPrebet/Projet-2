#include "tseitin.hpp"

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

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
		for (pair<int, unsigned int> _ : renaming)
		{
			Global::DEBUG(1) << "(" << _.first << " -> " << _.second << ")" << " ";
		}
		Global::DEBUG(1) << endl << endl;
	}

	/* TSEITIN */
	f_input.tseitin_x = renaming.new_variable();

	stack<Formula_input*> jobs;
	jobs.push(&f_input);

	list<Clause> clauses({});

	while(!jobs.empty())
	{
		Formula_input* f_top = jobs.top(); jobs.pop();

		f_top->tseitin_one_step(jobs, clauses, renaming);
	}

	if (Global::option.debug >= 1)
	{
		Global::DEBUG(1) << "Tseitin added the following variables to the mapping : " << endl;
		for (pair<int, unsigned int> _ : renaming)
		{
			//if (_.second >= Formula_input::nb_input_variables)
				Global::DEBUG(1) << "(" << _.first << " -> " << _.second << ")" << " ";
		}
		Global::DEBUG(1) << endl << endl;
	}

	clauses.push_back(list<int>({f_input.tseitin_x}));

	/* On construit la formule */
	Formula f(renaming);

	f.set_clauses_alive(clauses);

	return f;
}
