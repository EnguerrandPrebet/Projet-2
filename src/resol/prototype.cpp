#include "prototype.hpp"
#include "formula.hpp"

#include <list>
#include <map>
#include <vector>
#include <stack>

using namespace std;

State dpll(Formula& f, ostream& os, Option& option)
{
    pretreatment(f, os, option);

	stack<Decision_var> decisions;

    bool sat_unknown = true;
	int x = 0;
    while(sat_unknown)
	{
		Res action_result;
		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f,decisions,x,os,option);
		}while(action_result==NEW);

		switch(action_result)
		{
			case ERROR:
				DEBUG(1) << "Backtrack" << endl;
                sat_unknown = backtrack(f,decisions,os,option);
				break;
			case SUCCESS:
                sat_unknown = false;
				break;
			case NOTHING: //default
				x = get_next_var(f,os,option); //x = littéral
				DEBUG(1) << "x :" << x << endl;
				f.update_var(x,os,option); // met à jour assignment et vars_alive
				decisions.push(Decision_var(x,GUESS));
				break;
			default:
				break;
		}

		DEBUG(3) << "Stable state with";
        f.check(os, option);
	}

    return f.test(os,option);
}

bool backtrack(Formula& f, stack<Decision_var>& decisions, ostream& os, Option& option)
{
	vector<bool> be_cancelled(f.get_nb_Var()+1,false); //Variable à annuler
	while(!decisions.empty() && decisions.top().choice == INFER)
	{
		Decision_var dec = decisions.top();
		decisions.pop();
		DEBUG(1) << "Forget infer " << dec.var << endl;
		be_cancelled[abs(dec.var)] = true;
	}
	if(decisions.empty())
	{
		DEBUG(1) << "Nothing to backtrack" << endl;
		return false;
	}

	Decision_var change_of_mind = decisions.top();
	DEBUG(1) << "Forget guess " << change_of_mind.var << endl;
	be_cancelled[abs(change_of_mind.var)] = true;
	f.revive(os,option,be_cancelled);

	//Guess vers Infer
	change_of_mind.choice = INFER;
	change_of_mind.var *= -1;
	decisions.pop();
	decisions.push(change_of_mind);
	f.update_var(change_of_mind.var,os,option);

	return true;
}


void pretreatment(Formula& f, ostream& os, Option& option)
{
    f.supprTauto(os,option);
}

Res update(Formula& f, stack<Decision_var>& decisions, int& x, ostream& os, Option& option)
{
	DEBUG(1) << endl << "Update time !" << endl;
	f.apply_modification(x,os,option);//On met à jour les clauses ici
	DEBUG(1) << "End modif" << endl;

	f.check(os,option);

	Res act;
	if(option.watched_litterals == true)
		act = f.propagation_unitary_wl(decisions,os,option);
	else
		act = f.propagation_unitary(decisions,os,option);//On teste le résultat des modifications au passage
	DEBUG(1) << "After unitaire, act = " << act << endl;
	if(act == ERROR || act == SUCCESS)//Inutile d'aller plus loin
		return act;

    if(!option.watched_litterals)
	{
		Res act_aux = f.propagation_unique_polarity(decisions,os,option);
		if(act_aux != NOTHING)
			act = act_aux;
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
