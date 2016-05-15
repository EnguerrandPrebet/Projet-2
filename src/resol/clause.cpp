#include "clause.hpp"

#include "global.hpp"

#include <iostream>

#include <cassert>

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

std::stack<int>& Clause::get_stack()
{
	return stack_delete;
}

int Clause::get_first_litteral() const
{
	return literals_dyn.front();
}

std::vector<int>& Clause::get_lit_fixed()
{
	return literals_fixed;
}

unsigned int Clause::size(const std::vector<State>& assignment) const
{
	if (!Global::option.watched_litterals)
		return literals_dyn.size();
	else
	{
		assert(assignment.size() != 0);

		unsigned int nb = 0;

		unsigned int i1 = (nothing_before_wl) ? wl1 : 0;
		unsigned int i2 = (nothing_after_wl)  ? wl2 : (literals_dyn.size() - 1);

		for (unsigned int i = i1; i <= i2; i++)
		{
			int l = literals_fixed[i];
			unsigned int x = abs(l);

			if (assignment[x] == UNKNOWN)
				nb++;
			else /* La clause est satisfaite ? */
			{
				bool s_l = (l > 0);
				if ((bool)assignment[x] == s_l)
					return 0;
			}
		}

		return nb;
	}
}

std::list<int> Clause::get_vars_dyn() const
{
	return literals_dyn;
}

std::list<int> Clause::get_vars(const std::vector<State>& assignment) const
{
	if (!Global::option.watched_litterals)
	{
		return literals_dyn;
	}

	else /* wl */
	{
		assert(assignment.size() != 0);

		list<int> literals_dyn_from_fixed;

		unsigned int i1 = (nothing_before_wl) ? wl1 : 0;
		unsigned int i2 = (nothing_after_wl) ? wl2 : (literals_dyn_from_fixed.size() - 1);

		for (unsigned int i = i1; i <= i2; i++)
		{
			int l = literals_fixed[i];
			unsigned int x = abs(l);

			if (assignment[x] == UNKNOWN)
				literals_dyn_from_fixed.push_back(l);
		}

		return literals_dyn_from_fixed;
	}
}

State Clause::litteral_status(const vector<State>& assignment, int l) const
{
	unsigned int x = abs(l);
	switch (assignment[x])
	{
		case TRUE:
			return (l > 0) ? TRUE : FALSE;
		case FALSE:
			return (l < 0) ? TRUE : FALSE;
		default: /* UNKNOWN */
			return UNKNOWN;
	}
}

State Clause::check_satisfiability(const vector<State>& assignment)
{
	State sol_c = FALSE;
	for (int l : literals_dyn)
	{
		if (litteral_status(assignment, l) == TRUE)
			return TRUE;

		unsigned int x = abs(l);
		if (assignment[x] == UNKNOWN)
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
	if(wl2 == -1)
		return ERROR;

	Global::DEBUG(2) << "c pu: " << literals_fixed[wl1] << " " << literals_fixed[wl2] << endl;

	if (wl1 == wl2)
	{
		if(litteral_status(assignment,	literals_fixed[wl2]) == FALSE)
			return ERROR;
		else
		{
			x = literals_fixed[wl1]; // On met à jour x pour affecter sa valeur tranquillement
			return NEW;
		}
	}
	return NOTHING;
}

int Clause::time_max(std::vector<int> time_of_assign)
{
	int maxi = 0;
	for(int i:literals_fixed)
	{
		Global::DEBUG(2) << i << " " << time_of_assign[abs(i)] << endl;
		int x = abs(i);
		if(time_of_assign[x] > maxi)
			maxi = time_of_assign[x];
	}
	Global::DEBUG(2) << maxi << endl;
	return maxi;
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
