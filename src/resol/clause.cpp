#include "clause.hpp"

Clause::Clause()
{

}

State Clause::test(vector<State>& assignment)
{
	get_up_all();
	State sol_c = FALSE;
	for(auto x:vars)
	{
		if((assignment[abs(x)] == TRUE && (x > 0)) || (assignment[abs(x)] == FALSE && (x < 0)))
			return TRUE;
		if(assignment[abs(x)] == UNKNOWN)
			sol_c = UNKNOWN;
	}
	return sol_c;
}

bool Clause::apply_modification(vector<State>& assignment,ostream& os,Option& option)
{
	DEBUG(1) << "Modifying Clause :" << endl;
	for(auto it = vars.begin(); it != vars.end(); it++)
	{
		DEBUG(1) << "	Test var : " << (*it) << endl;
		switch(assignment[abs(*it)])
		{
			case FALSE:
				if(*it > 0)
				{
					DEBUG(1) << *it << " deleted from clause" << endl;
					stack_delete.push(*it);
					vars.erase(it);
					return false;
				}
				else
				{
					DEBUG(1) << *it << " make a clause true" << endl;
					return true;
				}
			case TRUE:
				if(*it < 0)
				{
					DEBUG(1) << *it << " deleted from clause" << endl;
					stack_delete.push(*it);
					vars.erase(it);
					return false;
				}
				else
				{
					DEBUG(1) << *it << " make a clause true" << endl;
					return true;
				}
			default:
				break;
		}
	}
	return false;
}

void Clause::get_up(int var,ostream& os,Option& option)
{
	if(stack_delete.top() == -var)
	{
		DEBUG(1) << "Revived " << -var << endl;
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
