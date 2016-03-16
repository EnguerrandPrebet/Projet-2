#include "formula.hpp"

#include <iostream>
#include <algorithm>
using namespace std;

Formula::Formula()
{

}
Formula::Formula(map<int,unsigned int> n_vars)
{
	var_true_name = n_vars;
	nb_Var = n_vars.size();
	var_alive = list<unsigned int>({});
	for(unsigned int i = 1; i <= nb_Var; i++)
		var_alive.push_back(i);
	assignment = vector<State>(nb_Var+1,UNKNOWN);
}

void Formula::update_var(int& x,ostream& os,Option& option)
{
	if(x>0)
		assignment[x] = TRUE;
	else
		assignment[-x] = FALSE;
	DEBUG(1) << abs(x) << " set to " << (x>0) << endl;

	auto it = find(var_alive.begin(),var_alive.end(),abs(x));
	DEBUG(1) << "Var " << x << " is in list = " << (it != var_alive.end()) << endl;
	if(it != var_alive.end())
	{
		var_alive.erase(it);
		DEBUG(1) << var_alive.size() << " vars left" << endl;
	}
	else
	{
		DEBUG(1) << "This should be already taken in account by another deduction" << endl;
	}
}

void Formula::apply_modification(int& x,ostream& os,Option& option)
{
	DEBUG(1) << "Modifying f" << endl;
	for(auto it = clauses_alive.begin(); it != clauses_alive.end();)
	{
		if(it->apply_modification(assignment,os,option))
		{
			DEBUG(1) << "Clause deleted while x = " << x << endl;
			stack_delete.push(Decision_cla(*it,x));
			clauses_alive.erase(it);
			if(clauses_alive.empty())
			{
				DEBUG(1) << "Il y a plus rien !" << endl;
				break;
			}
			it = clauses_alive.begin(); //erase de it fait des trucs chelou
		}
		else
		{
			it++;
		}

	}
}

State Formula::test(ostream& os,Option& option)
{
	revive(os,option); //Remet toutes les clauses vivantes
	State sol = TRUE;
	for(auto c:clauses_alive)
	{
		State sol_c = c.test(assignment);
		if(sol_c == UNKNOWN)
			return UNKNOWN;
		if(sol_c == FALSE)
			sol = FALSE;
	}
	return sol;
}

void Formula::supprTauto(ostream& os, Option& option)
{
	list<Clause> new_clauses({});
	for(auto c:clauses_alive)
	{
		c.test(assignment);
		map<int,bool> truc({}); //true : x, false : x bar
		bool tauto = false;
		for(auto j = c.get_vars().begin(); j != c.get_vars().end() && !tauto; j++)
		{
			if(truc.find(abs(*j)) != truc.end() && truc[abs(*j)] == (*j<0)) // l'opposé est présent
			{
				tauto = true;
				//break
			}
			else
			{
				truc[abs(*j)] = (*j>0);
			}
		}
		if(!tauto)
		{
			new_clauses.push_back(c);
		}
		else
		{
			DEBUG(1) << "Tautologie found" << endl;
		}
	}
	clear_c(new_clauses);
}

Res Formula::propagation_unitary(stack<Decision_var>& decisions, ostream& os, Option& option)
{
	Res act = SUCCESS;

	for(auto c:clauses_alive)
	{
		switch(c.size())
		{
			case 0:
				return ERROR;
			case 1:
				{
					act = NEW;
					int x = c.get(); /**Depend de l'implémentation**/
					DEBUG(1) << "Unitaire avec : " << x << endl;
					update_var(x,os,option);
					decisions.push(Decision_var(x,INFER));
				}break;
			default:
				if(act != NEW)
					act = NOTHING;
		}
	}

	return act;
}

Res Formula::propagation_unique_polarity(stack<Decision_var>& decisions, ostream& os, Option& option)
{
	vector<int> seen(nb_Var+1,0); //0 : Nothing spotted, 1 : x spotted, -1 : x bar spotted, 2 : both spotted

	for(auto c:clauses_alive)
	{
		for(auto j = c.get_vars().begin(); j!=c.get_vars().end(); j++)
		{
			if(seen[abs(*j)] == 2)
				continue;

			if(!seen[abs(*j)])
			{
				seen[abs(*j)] = (*j)/(abs(*j)); // +/- 1
			}
			else if((seen[abs(*j)] < 0) != ((*j) < 0)) //Different signs
			{
				seen[abs(*j)] = 2;
			}
		}
	}

	Res act = NOTHING;

	for(unsigned int i = 1; i <= nb_Var; i++)
	{
		if(abs(seen[i]) == 1)
		{
			int x = i*seen[i];
			DEBUG(1) << "Polarité unique avec : " << x << endl;
			update_var(x,os,option);
			decisions.push(Decision_var(x,INFER));
			act = NEW;
		}
	}
	return act;
}

void Formula::revive(ostream& os,  Option& option, int var)
{
	while(!stack_delete.empty() && (var == 0 || stack_delete.top().var == var))
	{
		DEBUG(1) << "Retour sur " << var << endl;
		if(var != 0)
			stack_delete.top().clause.get_up(var,os,option);
		clauses_alive.push_back(stack_delete.top().clause);
		stack_delete.pop();
	}
}

void Formula::clear_c(list<Clause> clauses)
{
	clauses_alive = clauses;
}

void Formula::check(ostream& os,Option& option,bool true_name)
{
	DEBUG(1) << endl << "Check :" << endl;
	for(auto c:clauses_alive)
	{
		for(auto it = c.get_vars().begin(); it != c.get_vars().end(); it++)
		{
			DEBUG(1) << ((true_name)?(int)var_true_name[abs(*it)]*(1-2*(*it<0)):(*it)) << " ";
		}
		DEBUG(1) << endl;
	}
	DEBUG(1) << "END" << endl << endl;
}

void Formula::print(Option& option)
{
	for(auto v:var_true_name)
	{
		switch(assignment[v.second])
		{
			case FALSE:
				cout << -1*v.first;
				break;
			case UNKNOWN:
				if(option.debug)
				cout << "?";
			case TRUE:
				cout << v.first;
				break;
		}
			cout << " ";
	}
	cout << endl;
}
