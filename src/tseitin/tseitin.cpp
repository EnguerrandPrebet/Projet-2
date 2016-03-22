#include "tseitin.hpp"

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

Formula tseitin(Formula_input& f_input, const Option& option, ostream& os)
{
	if (option.debug >= 1)
	{
		os << "Variables in the input formula: " << endl;
		for (pair<int, unsigned int> _ : f_input.variables_mapping)
		{

			os << "(" << _.first << ", " << _.second << ")" << " ";
		}
		os << endl << endl;
	}

	/* On récupère les clauses */
	f_input.x = Formula_input::next_available_var++;

	stack<Formula_input*> jobs;
	jobs.push(&f_input);

	list<Clause> clauses({});

	while(!jobs.empty())
	{
		Formula_input* f_top = jobs.top(); jobs.pop();

		f_top->tseitin_one_step(jobs, clauses);
	}

	/* On construit la formule */
	Formula f; //(variables_mapping)
	f.set_clauses_alive(clauses);

	//set<int> c({f_input.x});
	//final_formula.clauses.push_back(c);
	//final_formula.set_v(next_available_var-1);

	return f;
}
