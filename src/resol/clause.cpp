#include "clause.hpp"

#include <iostream>

using namespace std;

template<typename T> T abs(const T& x)
{
	return (x > 0) ? x : (-x);
}

Clause::Clause()
{

}

Clause::Clause(list<int> literals)
{
	/* non WL */
	literals_dyn = literals;

	/* WL */
    nothing_before_wl = true;
    nothing_after_wl = true;

	literals_fixed = vector<int>(literals.begin(), literals.end());
	wl1 = 0;
	wl2 = max(0, (int)literals_fixed.size() - 1);
}

State Clause::litteral_status(const vector<State>& assignment, int& l)
{
	unsigned int x = abs(l);
	switch(assignment[x])
	{
		case TRUE:
			if(l > 0)
				return TRUE;
			else
				return FALSE;
		case FALSE:
			if(l < 0)
				return TRUE;
			else
				return FALSE;
		case UNKNOWN:
			return UNKNOWN;
	}
	return UNKNOWN;//anti-warning
}

State Clause::check_satisfiability(const vector<State>& assignment)
{
	get_up_all();
	State sol_c = FALSE;
	for(int x : literals_dyn)
	{
		if(litteral_status(assignment,x) == TRUE)
			return TRUE;
		if(assignment[abs(x)] == UNKNOWN)
			sol_c = UNKNOWN;
	}
	return sol_c;
}

int Clause::apply_modification(vector<State>& assignment, ostream& os, const Option& option)
{
	DEBUG(3) << "Modifying Clause :" << endl;
	for(auto it = literals_dyn.begin(); it != literals_dyn.end();)
	{
		DEBUG(3) << "	Test var : " << (*it) << endl;
		switch(litteral_status(assignment,*it))
		{
			case FALSE:
				DEBUG(2) << *it << " deleted from clause" << endl;
				stack_delete.push(*it);
				literals_dyn.erase(it);
				it = literals_dyn.begin(); //erase a boulversé le pointeur it, on le remet au début par précaution
				break;
			case TRUE:
				DEBUG(2) << "A clause is true because of " <<  *it << endl;
				return *it;
				break;
			default:
				it++;
				break;
		}
	}
	return 0;
}

int Clause::apply_modification_wl(vector<State>& assignment,ostream& os, const Option& option)
{
	if(literals_fixed.empty())//CLause vide
		return 0;
	if(nothing_before_wl == false)
	{
		wl1 = 0;
		nothing_before_wl = true;
	}
	switch(litteral_status(assignment,literals_fixed[wl1]))
	{
		case FALSE:
			DEBUG(2) << literals_fixed[wl1] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment, literals_fixed[wl1]) == FALSE && wl1 != wl2)
			{
				wl1++;
			}
			break;
		case TRUE:
			DEBUG(2) << "A clause is true because of " <<  literals_fixed[wl1] << endl;
			return literals_fixed[wl1];
			break;
		default:
			break;
	}

	if(nothing_after_wl == false)
	{
		wl2 = max(0, (int)literals_fixed.size() - 1);
		nothing_after_wl = true;
	}
	switch(litteral_status(assignment, literals_fixed[wl2]))
	{
		case FALSE:
			DEBUG(2) << literals_fixed[wl2] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment, literals_fixed[wl2]) == FALSE && wl1 != wl2)
			{
				wl2--;
			}
			break;
		case TRUE:
			DEBUG(2) << "A clause is true because of " <<  literals_fixed[wl2] << endl;
			return literals_fixed[wl2];
			break;
		default:
			break;
	}
	DEBUG(2) << "c: " << literals_fixed[wl1] << " " << literals_fixed[wl2] << endl;
	return 0;
}

Res Clause::propagation_unitary_wl(vector<State>& assignment, ostream& os, const Option& option, int& x)
{
	if(wl1 == literals_fixed.size())
		return ERROR;
	DEBUG(2) << "c pu: " << literals_fixed[wl1] << " " << literals_fixed[wl2] << endl;

	if(wl1 == wl2)
	{
		if(litteral_status(assignment, literals_fixed[wl2]) == FALSE)
			return ERROR;
		else
		{
			x = literals_fixed[wl1]; // On met à jour x pour affecter sa valeur tranquillement
			return NEW;
		}
	}
	return NOTHING;
}
void Clause::get_up(vector<bool>& be_cancelled, ostream& os, const Option& option)
{
	while(!stack_delete.empty() && be_cancelled[abs(stack_delete.top())])
	{
		DEBUG(1) << "Revived " << stack_delete.top() << endl;
		literals_dyn.push_back(stack_delete.top());
		stack_delete.pop();
	}
}

void Clause::get_up_wl()
{
	nothing_before_wl = false;
	nothing_after_wl = false;
}

void Clause::print()
{
	cout << "c";
	for(int i : literals_dyn)
		cout << " " << i;
	cout << endl;
}

void Clause::get_up_all()
{
	while(!stack_delete.empty())
	{
		literals_dyn.push_back(stack_delete.top());
		stack_delete.pop();
	}
}
