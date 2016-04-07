#include "prototype.hpp"
#include "formula.hpp"
#include "cl.hpp"

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

	decisions.push(Decision_var(0, INFER, 0, _List_iterator<Clause>())); //Pour initialiser le temps tant qu'on ne met pas time en global (besoin
	while (sat_unknown)
	{
		Res action_result;
		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f, decisions, os, option);
		} while (action_result == NEW);

		switch (action_result)
		{
			case ERROR:
				DEBUG(1) << "Backtrack" << endl;
				sat_unknown = backtrack(f, decisions, os, option);
				break;

			case SUCCESS:
                sat_unknown = false;
				break;

			case NOTHING:
			{
				int l = get_next_assignment(f, os, option); // l = var & (true | false)
				DEBUG(1) << "x :" << l << endl;
				f.update_var(l, os, option); // met à jour assignment et vars_alive

				decisions.push(Decision_var({l, GUESS, decisions.top().time+1, _List_iterator<Clause>()}));
				break;
			}

			default:
				break;
		}

		DEBUG(3) << "Stable state with";
		f.print_formula(os, option);
	}

	return f.check_satisfiability(os, option);
}

bool backtrack(Formula& f, stack<Decision_var>& decisions, ostream& os, Option& option)
{
	int time_back = decisions.top().time; //Peut être réduit par le clause learning

	if(option.cl)
	{
		/*time_back = */clause_learning(f, decisions, os, option);
	}

	vector<bool> be_cancelled(f.nb_variables()+1, false); //Variables à annuler
	while(!decisions.empty() && (decisions.top().choice == INFER || decisions.top().time > time_back))
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
	change_of_mind.time = decisions.top().time;
	DEBUG(1) << "Back to time " << change_of_mind.time << endl;
	decisions.push(change_of_mind); //! GROS PROBLEME ICI ! mettre la clause obtenue via l'apprentissage
	f.update_var(change_of_mind.var,os,option);

	return true;
}


void pretreatment(Formula& f, ostream& os, const Option& option)
{
	f.remove_tautology(os, option);
}

Res update(Formula& f, stack<Decision_var>& decisions, ostream& os, const Option& option)
{
	DEBUG(1) << endl << "Update time !" << endl;
	f.apply_modification(decisions.top().time, os, option);//On met à jour les clauses ici
	DEBUG(1) << "End modif" << endl;

	f.print_formula(os, option);

	Res act;
	if(option.watched_litterals == true)
		act = f.propagation_unitary_wl(decisions, os, option);
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

int get_next_assignment(Formula& f, ostream& os, const Option& option)
{
	switch (option.heuristique)
	{
		case RAND:
			return f.get_random_var();

		case MOMS:
			return f.get_moms_var();

		case DLIS:
			return f.get_dlis_var();

		default: // NONE
			return f.get_first_var();
	}
}
