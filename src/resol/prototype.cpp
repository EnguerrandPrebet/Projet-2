#include "prototype.hpp"

#include "formula.hpp"
#include "cl.hpp"
#include "global.hpp"

#include <list>
#include <map>
#include <vector>
#include <stack>

using namespace std;

State dpll(Formula& f)
{
	pretreatment(f);

	stack<Decision_var> decisions;

	Theory theory(f.renaming.get_hi_smt());

	bool sat_unknown = true;

	decisions.push({0, 0, Clause(), INFER}); //Pour initialiser le temps tant qu'on ne met pas time en global (besoin
	while (sat_unknown)
	{
		Res action_result;
		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f, decisions, theory);
		} while (action_result == NEW);

		switch (action_result)
		{
			case ERROR:
				Global::DEBUG(1) << "Backtrack" << endl;
				sat_unknown = backtrack(f, decisions, theory);
				break;

			case SUCCESS:
				sat_unknown = false;
				break;

			case NOTHING:
			{
				int l = get_next_assignment(f); // l = var & (true | false)
				Global::DEBUG(1) << "x :" << l << " at " << decisions.top().time+1 << endl;
				f.update_var(l, theory); // met à jour assignment et vars_alive

				decisions.push({l, decisions.top().time+1, Clause(), GUESS});

				f.time(abs(l),decisions.top().time); //Met à jour time[abs(l)]
				break;
			}

			default:
				break;
		}

		Global::DEBUG(3) << "Stable state with";
		if (Global::option.debug >= 3)
			f.print_formula();
	}

	return f.check_satisfiability();
}

bool backtrack(Formula& f, stack<Decision_var>& decisions, Theory& theory)
{
	int time_back = decisions.top().time - 1; //Peut être réduit par le clause learning

	if(Global::option.cl)
	{
		time_back = clause_learning(f, decisions);
	}
	decisions.pop(); //On enlève "infer 0" utilisé dans le CL

	vector<bool> be_cancelled(f.nb_variables()+1, false); //Variables à annuler
	while(!decisions.empty() && (decisions.top().choice == INFER || decisions.top().time - 1 > time_back))
	{
		Decision_var dec = decisions.top();
		decisions.pop();
		Global::DEBUG(1) << "Forget " << ((dec.choice == INFER)?"infer":"guess") << " " << dec.var << endl;

		if(Global::option.smt)
				theory.backtrack(f.renaming.inverse_translate_litteral(dec.var));

		be_cancelled[abs(dec.var)] = true;
	}

	if(decisions.empty())
	{
		Global::DEBUG(1) << "Nothing to backtrack" << endl;
		return false;
	}

	Decision_var change_of_mind = decisions.top();
	decisions.pop();
	Global::DEBUG(1) << "Forget guess " << change_of_mind.var << endl;
	if(Global::option.smt)
				theory.backtrack(f.renaming.inverse_translate_litteral(change_of_mind.var));
	be_cancelled[abs(change_of_mind.var)] = true;
	f.revive(be_cancelled);

	if(!Global::option.cl) //La nouvelle clause va faire la déduction seule. Ceci est inutile donc
	{
		//Guess vers Infer
		change_of_mind.choice = INFER;
		change_of_mind.var *= -1;
		decisions.top();
		change_of_mind.time = time_back;
		change_of_mind.reason = Clause();

		decisions.push(change_of_mind);
		f.update_var(change_of_mind.var, theory);
	}

	Global::DEBUG(1) << "Back to time " << time_back << endl;

	return true;
}


void pretreatment(Formula& f)
{
	f.remove_tautology();
}

Res update(Formula& f, stack<Decision_var>& decisions, Theory& theory)
{
	Global::DEBUG(1) << endl << "Update time !" << endl;
	f.apply_modification(decisions.top().time);//On met à jour les clauses ici
	Global::DEBUG(1) << "End modif" << endl;


	if (Global::option.debug >= 3)
		f.print_formula();

	Res act;
	if(Global::option.watched_litterals == true)
		act = f.propagation_unitary_wl(decisions, theory);
	else
		act = f.propagation_unitary(decisions, theory);//On teste le résultat des modifications au passage

	Global::DEBUG(1) << "After unitaire, act = " << act << endl;
	if(act == ERROR || act == SUCCESS)//Inutile d'aller plus loin
		return act;

	if(!Global::option.watched_litterals && !Global::option.cl)
	{
		Res act_aux = f.propagation_unique_polarity(decisions, theory);
		if(act_aux != NOTHING)
			act = act_aux;
	}

	return act;
}

int get_next_assignment(const Formula& f)
{
	Global::DEBUG(2) << Global::option.heuristique << endl;
	switch (Global::option.heuristique)
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
