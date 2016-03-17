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
- f.nbVar
- "list clauses"
- add(list clauses, Clause clause);
- clause.get()
- clause.size()
*/
void dpll(Formula& f, ostream& os, Option& option)
{
	pretreatment(f,os,option);

	stack<Decision_var> decisions;

	bool sat_unk = true;
	int x = 0;
	while(sat_unk)
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
				sat_unk = backtrack(f,decisions,os,option);
				f.check(os,option);
				break;
			case SUCCESS:
				sat_unk = false;
				break;
			case NOTHING: //default
				x = get_next_var(f,os,option); //x = littéral
				DEBUG(1) << "x :" << x << endl;
				f.check(os,option);
				f.update_var(x,os,option); // met à jour assignment et vars_alive
				decisions.push(Decision_var(x,GUESS));
			default:
				break;
		}

		//DEBUG(3) << "Stable state with";
		//f.check(os,option);


	}

	//Peut être hors de la fonction DPLL (même si je ne vois pas pourquoi)
	switch(f.test(os,option))
	{
		case TRUE:
			cout << "s SATISFIABLE" << endl;
			f.print(option);
			break;
		case FALSE:
			cout << "s UNSATISFIABLE" << endl;
			break;
		case UNKNOWN:
			cout << "s ???" << endl;
	}
}

bool backtrack(Formula& f, stack<Decision_var>& decisions, ostream& os, Option& option)
{
	vector<bool> be_cancelled(f.get_nb_Var()+1,false);
	while(!decisions.empty() && decisions.top().choice == INFER)
	{
		Decision_var dec = decisions.top();
		decisions.pop();
		DEBUG(1) << "Forget infer " << dec.var << endl;
		be_cancelled[abs(dec.var)] = true;

	}
	if(decisions.empty())
	{
		//DEBUG(1) << "Nothing to backtrack" << endl;
		return false;
	}

	Decision_var change_of_mind = decisions.top();
	DEBUG(1) << "Forget guess " << change_of_mind.var << endl;
	be_cancelled[abs(change_of_mind.var)] = true;
	f.revive(os,option,be_cancelled);
	change_of_mind.choice = INFER;
	change_of_mind.var *= -1;

	decisions.pop();
	decisions.push(change_of_mind);
	f.update_var(change_of_mind.var,os,option);

	return true;
}


void pretreatment(Formula& f, ostream& os, Option& option)
{
	/**Redondance = avant ?**/
    f.supprTauto(os,option);
}

Res update(Formula& f, stack<Decision_var>& decisions,int& x, ostream& os, Option& option)
{
	//DEBUG(1) << endl << "Update time !" << endl;
	f.apply_modification(x,os,option);//On met à jour les clauses ici
	//DEBUG(1) << "End modif" << endl;
	//f.check(os,option);
	Res act = f.propagation_unitary(decisions,os,option); //On teste le résultat des modifications au passage
	//DEBUG(1) << "After unitaire, act = " << act << endl;
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
