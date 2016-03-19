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


	//WL
    nothing_before_wl = true;
    nothing_after_wl = true;

	vars_tab = vector<int>(vars.begin(),vars.end());
	wl1 = 0;
    wl2 = max(0,(int)vars_tab.size()-1);
}

State Clause::litteral_status(vector<State>& assignment, int& x)
{
	switch(assignment[abs(x)])
	{
		case TRUE:
			if(x > 0)
				return TRUE;
			else
				return FALSE;
		case FALSE:
			if(x < 0)
				return TRUE;
			else
				return FALSE;
		case UNKNOWN:
			return UNKNOWN;
	}
	return UNKNOWN;//anti-warning
}

State Clause::test(vector<State>& assignment)
{
	get_up_all();
	State sol_c = FALSE;
	for(auto x:vars)
	{
		if(litteral_status(assignment,x) == TRUE)
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
		switch(litteral_status(assignment,*it))
		{
			case FALSE:
				DEBUG(2) << *it << " deleted from clause" << endl;
				stack_delete.push(*it);
				vars.erase(it);
				it = vars.begin(); //erase a boulversé le pointeur it, on le remet au début par précaution
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

int Clause::apply_modification_wl(vector<State>& assignment,ostream& os, Option& option)
{
	if(vars.empty())//CLause vide
		return 0;
	if(nothing_before_wl == false)
	{
		wl1 = 0;
		nothing_before_wl = true;
	}
	switch(litteral_status(assignment,vars_tab[wl1]))
	{
		case FALSE:
			DEBUG(2) << vars_tab[wl1] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment,vars_tab[wl1]) == FALSE && wl1 != wl2)
			{
				wl1++;
			}
			break;
		case TRUE:
			DEBUG(2) << "A clause is true because of " <<  vars_tab[wl1] << endl;
			return vars_tab[wl1];
			break;
		default:
			break;
	}

	if(nothing_after_wl == false)
	{
		wl2 = max(0,(int)vars.size()-1);
		nothing_after_wl = true;
	}
	switch(litteral_status(assignment,vars_tab[wl2]))
	{
		case FALSE:
			DEBUG(2) << vars_tab[wl2] << " \"deleted\" from clause" << endl;

			while(litteral_status(assignment,vars_tab[wl2]) == FALSE && wl1 != wl2)
			{
				wl2--;
			}
			break;
		case TRUE:
			DEBUG(2) << "A clause is true because of " <<  vars_tab[wl2] << endl;
			return vars_tab[wl2];
			break;
		default:
			break;
	}
	DEBUG(2) << "c: " << vars_tab[wl1] << " " << vars_tab[wl2] << endl;
	return 0;
}

Res Clause::propagation_unitary_wl(vector<State>& assignment, ostream& os, Option& option, int& x)
{
	if(wl1 == vars_tab.size())
		return ERROR;
	DEBUG(2) << "c pu: " << vars_tab[wl1] << " " << vars_tab[wl2] << endl;

	if(wl1 == wl2)
	{
		if(litteral_status(assignment,vars_tab[wl2]) == FALSE)
			return ERROR;
		else
		{
			x = vars_tab[wl1]; //On met à jour x pour affecter sa valeur tranquillement
			return NEW;
		}
	}
	return NOTHING;
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

void Clause::get_up_wl()
{
	nothing_before_wl = false;
	nothing_after_wl = false;
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
