#include <iostream>
#include "formula.hpp"
#include "prototype.hpp"
#include <list>
#include <map>
#include <vector>
#include <stack>
using namespace std;

/*
Dépend de l'implémentation :
- f.update_var()
- f.size
- "list clauses"
- add(list clauses, Clause clause);
- clause.get()
- clause.size()
*/
void dpll(Formula& f, ostream& os, Option& option)
{

	pretreatment(f,os,option);

	stack<Decision> decisions;

	bool sat_unk = true;
	while(sat_unk)
	{
		Res action_result;

		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f,decisions,os,option);
		}while(action_result==NEW);

		switch(action_result)
		{
			case ERROR:
				sat_unk = backtrack(f,decisions,os,option);
				break;
			case SUCCESS:
				sat_unk = false;
				break;
			case NOTHING: //default
				int x = get_next_var(f,os,option); //x = littéral
				f.update_var(x,os,option); // met à jour assignment et vars_alive
				decisions.push(Decision(x,GUESS));
		}



	}

	//Peut être hors de la fonction DPLL (même si je ne vois pas pourquoi)
	switch(f.test())
	{
		case TRUE:
			cout << "s SATISFIABLE" << "blabla";
			break;
		case FALSE:
			cout << "s UNSATISFIABLE";
			break;
		case UNKNOWN:
			cout << "s ???";
	}
}

bool backtrack(Formula& f, stack<Decision>& decisions, ostream& os, Option& option)
{
	do
	{
		const Decision dec = decisions.top();
		decisions.pop();
		f.revive(dec.var,os,option.debug);

	}while(!decisions.empty() && decisions.top().choice == INFER);
	if(decisions.empty())
	{
		return ERROR;
	}

	Decision change_of_mind = decisions.top();
	change_of_mind.choice = INFER;
	change_of_mind.var *= -1;

	decisions.pop();
	decisions.push(change_of_mind);
	f.update_var(change_of_mind.var,os,option);

	return NOTHING;
}

State Formula::test()
{
	revive(); //Remet toutes les clauses vivantes
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
void pretreatment(Formula& f, ostream& os, Option& option)
{
	/**Redondance / Renommage  = avant ?**/
	/**Pour le cnf, suffit de créer un vector**/
    f.supprTauto(os,option);
}

void Formula::supprTauto(ostream& os, Option& option)
{
	list<Clause> new_clauses({});
	for(auto c:clauses_alive)
	{
		map<int,bool> truc;/**réinitialisation ?**/ //true : x, false : x bar
		bool tauto = false;
		for(auto j = c.get_vars().begin(); j != c.get_vars().end() && !tauto; j++)
		{
			if(truc.find(abs(*j)) != truc.end() && truc[abs(*j)] == (*j<0)) // l'opposé est présent
			{
				tauto = true;
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
	}
	clear_c(new_clauses);
}

Res update(Formula& f, stack<Decision>& decisions, ostream& os, Option& option)
{
	f.apply_modification(os,option);//On met à jour les clauses ici

	Res act = f.propagation_unitary(decisions,os,option); //On teste le résultat des modifications au passage
	if(act == ERROR || act == SUCCESS)
		return act;

	if(!option.lw)
	{
		Res act_aux = f.propagation_unique_polarity(decisions,os,option);
		if(act_aux != NOTHING)
			act = act_aux;
	}

	return act;
}

Res Formula::propagation_unitary(stack<Decision>& decisions, ostream& os, Option& option)
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
					update_var(x,os,option);
					decisions.push(Decision(x,INFER));
				}break;
			default:
				if(act != NEW)
					act = NOTHING;
		}
	}

	return act;
}

Res Formula::propagation_unique_polarity(stack<Decision>& decisions, ostream& os, Option& option)
{
	vector<int> seen(size+1,0); //0 : Nothing spotted, 1 : x spotted, -1 : x bar spotted, 2 : both spotted

	for(auto c:clauses_alive)
	{
		bool tauto = false;
		for(auto j = c.get_vars().begin(); j!=c.get_vars().end() && !tauto; j++)
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

	for(int i = 1; i <= size; i++)
	{
		if(abs(seen[i]) == 1)
		{
			int x = i*seen[i];
			update_var(x,os,option);
			decisions.push(Decision(x,INFER));
			act = NEW;
		}
	}

	return act;
}

int get_next_var(Formula& f, ostream& os, Option& option)
{
	int x;
	switch(option.get)
	{
		default:
			x = f.get_fst_var();
	}
	return x;
}

void Formula::revive(int var, ostream& os, int debug)
{
	while((var == 0 && !stack_delete.empty()) || stack_delete.top().var == var)
	{
		stack_delete.top().clause.get_up(var);
		stack_delete.pop();
	}
}

void Formula::clear_c(list<Clause> clauses)
{
	clauses_alive = clauses;
}
