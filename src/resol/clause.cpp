#include "clause.hpp"

#include "global.hpp"

#include <iostream>

using namespace std;

template<typename T> T abs(const T& x)
{
	return (x > 0) ? x : (-x);
}

Clause::Clause()
{

}

Clause::Clause(const list<int>& literals)
{
	/* non WL */
	literals_dyn = literals;

	/* WL */
	nothing_before_wl = true;
	nothing_after_wl = true;

	literals_fixed = vector<int>(literals.begin(), literals.end());
	wl1 = 0;
	wl2 = (int)literals_fixed.size() - 1;
}

State Clause::litteral_status(const vector<State>& assignment, int l) const
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

int Clause::apply_modification(const vector<State>& assignment)
{
	Global::DEBUG(3) << "Modifying Clause :" << endl;

	for (auto it = literals_dyn.begin(); it != literals_dyn.end();)
	{
		Global::DEBUG(3) << "	Test var : " << (*it) << endl;
		switch(litteral_status(assignment,*it))
		{
			case FALSE:
				Global::DEBUG(2) << *it << " deleted from clause" << endl;
				stack_delete.push(*it);
				it = literals_dyn.erase(it);
				break;
			case TRUE:
				Global::DEBUG(2) << "A clause is true because of " <<  *it << endl;
				return *it;
				break;
			default:
				it++;
				break;
		}
	}
	return 0;
}

int Clause::apply_modification_wl(const vector<State>& assignment)
{
	if(wl2 == -1)//Clause vide
		return 0;
	if(nothing_before_wl == false)
	{
		wl1 = 0;
		nothing_before_wl = true;
	}
	switch(litteral_status(assignment,literals_fixed[wl1]))
	{
		case FALSE:
			Global::DEBUG(2) << literals_fixed[wl1] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment, literals_fixed[wl1]) == FALSE && wl1 != wl2)
			{
				wl1++;
			}
			break;
		case TRUE:
			Global::DEBUG(2) << "A clause is true because of " <<  literals_fixed[wl1] << endl;
			return literals_fixed[wl1];
			break;
		default:
			break;
	}

	if(nothing_after_wl == false)
	{
		wl2 = (int)literals_fixed.size() - 1;
		nothing_after_wl = true;
	}
	switch(litteral_status(assignment, literals_fixed[wl2]))
	{
		case FALSE:
			Global::DEBUG(2) << literals_fixed[wl2] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment, literals_fixed[wl2]) == FALSE && wl1 != wl2)
			{
				wl2--;
			}
			break;
		case TRUE:
			Global::DEBUG(2) << "A clause is true because of " <<  literals_fixed[wl2] << endl;
			return literals_fixed[wl2];
			break;
		default:
			break;
	}
	Global::DEBUG(2) << "c: " << literals_fixed[wl1] << " " << literals_fixed[wl2] << endl;
	return 0;
}

Res Clause::propagation_unitary_wl(const vector<State>& assignment, int& x)
{
	if(wl1 == -1)
		return ERROR;
	Global::DEBUG(2) << "c pu: " << literals_fixed[wl1] << " " << literals_fixed[wl2] << endl;

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
void Clause::get_up(const vector<bool>& be_cancelled)
{
	while(!stack_delete.empty() && be_cancelled[abs(stack_delete.top())])
	{
		Global::DEBUG(1) << "Revived " << stack_delete.top() << endl;
		literals_dyn.push_back(stack_delete.top());
		stack_delete.pop();
	}
}

void Clause::get_up_wl()
{
	nothing_before_wl = false;
	nothing_after_wl = false;
}

void Clause::print() const
{
	Global::DEBUG() << "c";
	for(int i : literals_dyn)
		Global::DEBUG() << " " << i;
	Global::DEBUG() << endl;
}

void Clause::get_up_all()
{
	while(!stack_delete.empty())
	{
		literals_dyn.push_back(stack_delete.top());
		stack_delete.pop();
	}
}
