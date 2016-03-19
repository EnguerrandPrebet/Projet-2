#include "formula.hpp"

#include <algorithm>
#include <iostream> //Ne compile pas sans

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

	tab_stack_delete = vector<list<Clause>>(nb_Var+1,list<Clause>{});
}

void Formula::update_var(int& x,ostream& os,Option& option)
{
	if(x>0)
		assignment[x] = TRUE;
	else
		assignment[-x] = FALSE;
	DEBUG(1) << abs(x) << " set to " << (x>0) << endl;

	auto it = find(var_alive.begin(),var_alive.end(),abs(x));
	var_alive.erase(it);
	DEBUG(1) << var_alive.size() << " vars left" << endl;
}

void Formula::apply_modification(int& x,ostream& os,Option& option)
{
	DEBUG(1) << "Modifying f" << endl;
	for(auto it = clauses_alive.begin(); it != clauses_alive.end();)
	{
		int cause;
		if((option.watched_litterals == false && (cause = it->apply_modification(assignment,os,option))) || (option.watched_litterals == true && (cause = it->apply_modification_wl(assignment,os,option))))
		{
			DEBUG(1) << "Clause deleted while x = " << x << " because of " << cause <<  endl;
			tab_stack_delete[abs(cause)].push_back(*it);
			clauses_alive.erase(it);
			if(clauses_alive.empty())
			{
				DEBUG(1) << "Il y a plus rien !" << endl;
				break;
			}
			it = clauses_alive.begin(); //erase fait des choses bizarres à it, on remet au début par sécurité
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
					int x = c.get();
					DEBUG(1) << "Unitaire avec : " << x << endl;
					if(assignment[abs(x)] == UNKNOWN) //Si une autre déduction de ce parcours ne l'a pas modifié
					{
						update_var(x,os,option);
						decisions.push(Decision_var(x,INFER));
					}
				}break;
			default:
				if(act != NEW)
					act = NOTHING;
		}
	}

	return act;
}

Res Formula::propagation_unitary_wl(stack<Decision_var>& decisions, ostream& os, Option& option)
{
	Res act = SUCCESS;

	for(auto c:clauses_alive)
	{
		int x;
		Res act_aux = c.propagation_unitary_wl(assignment,os,option,x);
		if(act_aux == NEW)
		{
			DEBUG(1) << "Unitaire avec : " << x << endl;
			if(assignment[abs(x)] == UNKNOWN) //Si une autre déduction de ce parcours ne l'a pas modifié
			{
				update_var(x,os,option);
				decisions.push(Decision_var(x,INFER));
			}
			act = NEW;
		}
		else if(act_aux == ERROR)
			return ERROR;

		if(act != NEW)
			act = NOTHING;
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
			act = NEW;

			DEBUG(1) << "Polarité unique avec : " << x << endl;
			if(assignment[abs(x)] == UNKNOWN)
			{
				update_var(x,os,option);
				decisions.push(Decision_var(x,INFER));
			}
		}
	}
	return act;
}

void Formula::revive(ostream& os,  Option& option, vector<bool> be_cancelled)
{
	unsigned int taille = be_cancelled.size();

	for(unsigned int i = 1; i < taille; i++)//On remet les variables
	{
		if(be_cancelled[i])
		{
			var_alive.push_back(i);
			assignment[i] = UNKNOWN;
		}
	}

	for(unsigned int i = 1;i<taille;i++)
	{
		if(be_cancelled[i])
		{
			clauses_alive.splice(clauses_alive.end(),tab_stack_delete[i]);
		}
	}

	if(taille != 1)
	{
		for(auto it = clauses_alive.begin(); it != clauses_alive.end(); it++)//Puis les variables dans les clauses
		{
			if(option.watched_litterals == false)
				it->get_up(be_cancelled,os,option);
			else
				it->get_up_wl();
		}
	}
}

void Formula::clear_c(list<Clause> clauses)
{
	clauses_alive = clauses;
}

void Formula::print_formula(ostream& os, const Option& option, bool true_name)
{
	DEBUG(3) << endl << "Check :" << endl;
    for(auto c : clauses_alive)
    {
		for(auto it = c.get_vars().begin(); it != c.get_vars().end(); it++)
		{
			DEBUG(3) << ((true_name)?(int)var_true_name[abs(*it)]*(1-2*(*it<0)):(*it)) << " ";
		}
		DEBUG(3) << endl;
	}
	DEBUG(3) << "END" << endl << endl;
}

void Formula::print_assignment(const Option& option, ostream& os)
{
    for(auto v : var_true_name)
	{
		switch(assignment[v.second])
		{
			case FALSE:
				cout << -1*v.first;
				break;

			case UNKNOWN:
				if(option.debug)
				cout << "?";
				/**Pas de break et dans cet ordre, il faut quand même afficher le nom de la variable (ex : "?1")**/
			case TRUE:
				cout << v.first;
				break;
		}

        cout << ' ';
	}

	cout << endl;
}
