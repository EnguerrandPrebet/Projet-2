#include "clause.hpp"

Clause::Clause()
{
	vars = list<int>({});
}

State Clause::test(vector<State>& assignment)
{
	get_up();
	bool sol_c = FALSE;
	for(auto x:vars)
	{
		if((assignment[abs(x)] == TRUE && (x > 0)) || (assignment[abs(x)] == FALSE && (x < 0)))
			return TRUE;
		if(assignment[abs(x)] == UNKNOWN)
			sol_c = UNKNOWN;
	}
	return UNKNOWN;
}
