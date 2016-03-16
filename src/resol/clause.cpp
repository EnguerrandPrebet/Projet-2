#include "clause.hpp"

Clause::Clause()
{

}

State Clause::test(vector<State>& assignment)
{
	get_up_all();
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

bool Clause::apply_modification(int& x,ostream& os,Option& option)
{
	for(auto it = vars.begin(); it != vars.end(); it++)
	{
		if(x == -(*it))
		{
			stack_delete.push(*it);
			vars.erase(it);
			return false;
		}
		if(x == (*it))
		{
			return true;
		}
	}
	return false;
}

void Clause::get_up(int var)
{
	if(stack_delete.top() == -var)
	{
		vars.push_back(-var);
		stack_delete.pop();
	}
}

void Clause::get_up_all()
{
	while(!stack_delete.empty())
	{
		vars.push_back(stack_delete.top());
		stack_delete.pop();
	}
}
