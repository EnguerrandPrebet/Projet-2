#include "clause.hpp"

#include <iostream> //Ne compile sans

using namespace std;

template<typename T> T abs(const T& x)
{
	return (x > 0) ? x : (-x);
}

Clause::Clause()
{

}

Clause::Clause(list<int> n_vars)
{
    vars = n_vars;
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

int Clause::apply_modification(vector<State>& assignment,ostream& os,Option& option)
{
	DEBUG(3) << "Modifying Clause :" << endl;
	for(auto it = vars.begin(); it != vars.end();)
	{
		DEBUG(3) << "	Test var : " << (*it) << endl;
		switch(assignment[abs(*it)])
		{
			case FALSE:
				if(*it > 0)
				{
					DEBUG(2) << *it << " deleted from clause" << endl;
					stack_delete.push(*it);
					vars.erase(it);
					it = vars.begin(); //erase a boulversé le pointeur it, on le remet au début par précaution
				}
				else
				{
					DEBUG(2) << "A clause is true because of " <<  *it << endl;
					return *it;
				}
				break;
			case TRUE:
				if(*it < 0)
				{
					DEBUG(2) << *it << " deleted from clause" << endl;
					stack_delete.push(*it);
					vars.erase(it);
					it = vars.begin();//De meme
				}
				else
				{
					DEBUG(2) << "A clause is true because of " <<  *it << endl;
					return *it;
				}
				break;
			default:
				it++;
				break;
		}
	}
	return 0;
}

void Clause::get_up(vector<bool>& be_cancelled,ostream& os,Option& option)
{
	while(!stack_delete.empty() && be_cancelled[abs(stack_delete.top())])
	{
		DEBUG(1) << "Revived " << stack_delete.top() << endl;
		vars.push_back(stack_delete.top());
		stack_delete.pop();
	}
}

void Clause::print()
{
	cout << "c";
	for(int i:vars)
		cout << " " << i;
	cout << endl;
}

void Clause::get_up_all()
{
	while(!stack_delete.empty())
	{
		vars.push_back(stack_delete.top());
		stack_delete.pop();
	}
}
