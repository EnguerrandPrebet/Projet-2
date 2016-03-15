#include "dpll_solver_utils.hpp"

#include <set>

using namespace std;

choice not_choice(choice x)
{
	switch (x)
	{
		case TRUE:
			return FALSE;
		case FALSE:
			return TRUE;
		default:
			return x;
	}
}

choice truth_value(int l, const vector<choice>& assignments)
{
	if (l > 0)
		return assignments[l];
	else /* (l <= 0) */
		return not_choice(assignments[-l]);
}

choice satisfy_litteral(int l)
{
	if (l > 0)
		return TRUE;
	else /* (l <= 0) */
		return FALSE;
}

Formula pretreatment(vector<choice>& assignments, Formula f, ostream& os, int debug)
{
	Formula new_f;
	
	for (unsigned int k = 0; k < f.c(); k++)
	{
		/* Clause unitaire ? */
		if (f.clauses[k].size() == 1)
		{
			if(debug)
			{
				os << "Unit clause detected" << endl;
				os << "Clause C_" << k+1 << " deleted." << endl;
				os << "Litteral " << *(f.clauses[k].begin()) << " is now true." << endl;
			}
			assignments[abs(*(f.clauses[k].begin()))] = satisfy_litteral(*(f.clauses[k].begin()));
		}
		else /* Clause tautologique ? */
		{
			set<int> redundancy;
			for (auto l:f.clauses[k])
			{
				/* une clause qui contient x et -x est une tautologie */

				if (redundancy.find(-l) == redundancy.end())
					redundancy.insert(l);

				else
				{
					if(debug)
						os << "Tautology found" << endl << "Clause C_" << k+1 << "deleted." << endl;
					goto next_clause;
				}
			}
			
				new_f.clauses.push_back(f.clauses[k]);
		}

		next_clause:;
	}
	
	/* Polarité unique */
	set<int> variables;
	
	for (unsigned int k = 0; k < new_f.c(); k++)
		for (auto l : f.clauses[k])
			variables.insert(l);
	
	for (auto l : variables)
	{
		bool unique_polarity = (variables.find(-l) == variables.end());

		if (unique_polarity)
		{
			if(debug)
				os << "Unique polarity: " << l << endl;
				
			assignments[abs(l)] = satisfy_litteral(l);
		}
	}
	
	return new_f;
}
