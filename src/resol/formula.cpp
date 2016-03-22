#include "formula.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include <climits> // pour initialisation des min / max

using namespace std;

Formula::Formula()
{

}

Formula::Formula(map<int, unsigned int> n_vars)
{
	var_true_name = n_vars;
	nb_Var = n_vars.size();
	var_alive = list<unsigned int>({});
	for(unsigned int i = 1; i <= nb_Var; i++)
		var_alive.push_back(i);
	assignment = vector<State>(nb_Var+1,UNKNOWN);

	tab_stack_delete = vector<list<Clause>>(nb_Var+1,list<Clause>{});
}

int Formula::get_first_var() const
{
	return var_alive.front();
}

int Formula::get_random_var() const
{
	static default_random_engine rng;

	/* Initialisation du générateur de nombres aléatoires */
	static bool rng_initialise = false;
	if (!rng_initialise)
	{
		rng.seed(random_device()());
		rng_initialise = true;
	}

	/* On génère le choix */
	unsigned int nb_unknown = var_alive.size();

	uniform_int_distribution<default_random_engine::result_type> uni_dist(0, nb_unknown-1); // [| a; b |]

	/* On trouve la variable correspondante */
	unsigned int count = uni_dist(rng);
	auto it = next(var_alive.begin(), count);
	int x = *it;

	/* On génère / trouve sa valeur */
	uniform_int_distribution<default_random_engine::result_type> binary_dist(0, 1);
	int sign = (binary_dist(rng) == 0) ? 1 : -1;

	//cout << "RANDOM :" << sign * x << ' ' << var_alive.size() << endl;

	return sign * x;
}

int Formula::get_moms_var() const
{
	/* On trouve la taille minimale des clauses */
	unsigned int min_clause_size = UINT_MAX;
	for (const Clause& clause : clauses_alive)
		min_clause_size = min(min_clause_size, clause.size());

	/* On cherche le littéral qui apparaît le plus souvent dans ces clauses */
	int max_x = 1; // initialisation sans intérêt, pour enlever le warning
	unsigned int max_freq = 0;

	map<int, unsigned int> frequencies;
	for (const Clause& clause : clauses_alive)
		if (clause.size() == min_clause_size)
			for (int x : clause.get_vars())
			{
				unsigned int new_freq_x = (++frequencies[x]); // si frequencies[x] n'est pas présent, il est initialisé à 0

				if (new_freq_x > max_freq)
				{
					max_x = x;
					max_freq = new_freq_x;
				}
			}

	return max_x;
}

int Formula::get_dlis_var() const
{
	/* On cherche l'assignation qui rend le plus de clauses valides */
	map<int, unsigned int> variables_score;

	for (const Clause& clause : clauses_alive)
		for (int l : clause.get_vars()) // chaque littéral doit apparaître de façon unique dans une clause
			variables_score[l]++; // 'l' satisfait 'clause'
			/* remarque : si variables_score[l] n'existe pas encore il est initialisé à 0 */

	/* On récupère le littéral de meilleur score */
	const auto& max_score = max_element(variables_score.begin(), variables_score.end(),
													[](const pair<int, unsigned int>& p1, const pair<int, unsigned int>& p2)
													 { return p1.second < p2.second; });
	return max_score->first;
}

void Formula::update_var(int& x,ostream& os,Option& option)
{
	if (x>0)
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
			sol_c = UNKNOWN;
		if(sol_c == FALSE)
			return FALSE;
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
	set_clauses_alive(new_clauses);
}

Res Formula::propagation_unitary(stack<Decision_var>& decisions, ostream& os, Option& option)
{
	Res act = SUCCESS;

	for(auto c = clauses_alive.begin(); c != clauses_alive.end(); c++)
	{
		switch(c->size())
		{
			case 0:
				return ERROR;
			case 1:
				{
					act = NEW;
					int x = c->get();
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

	for(auto c = clauses_alive.begin(); c != clauses_alive.end(); c++)
	{
		int x;
		Res act_aux = c->propagation_unitary_wl(assignment,os,option,x);
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

	for(auto c = clauses_alive.begin(); c != clauses_alive.end(); c++)
	{
		for(auto j = c->get_vars().begin(); j!=c->get_vars().end(); j++)
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

void Formula::set_clauses_alive(list<Clause> clauses)
{
	clauses_alive = clauses;
}

void Formula::print_formula(ostream& os, const Option& option, bool true_name, unsigned int debug_lvl)
{
	if(option.debug > debug_lvl)
	{
		os << endl << "Check :" << endl;
		for(auto c : clauses_alive)
		{
			for(auto it = c.get_vars().begin(); it != c.get_vars().end(); it++)
			{
				os << ((true_name)?(int)var_true_name[abs(*it)]*(1-2*(*it<0)):(*it)) << " ";
			}
			os << endl;
		}
		os << "END" << endl << endl;
	}
}

void Formula::print_assignment(const Option& option, ostream& os, bool tseitin, unsigned int tseitin_nb_input_variables)
{
	for (const pair<int, unsigned int> _ : var_true_name)
	{
		int x = _.first;
		unsigned int mapped_x = _.second;

		/* Variable qui n'était pas dans l'input */
		//if (tseitin && mapped_x > tseitin_nb_input_variables)
		//	continue;

		switch (assignment[mapped_x])
		{
			case TRUE:
				cout << x;
				break;

			case FALSE:
				cout << (-x);
				break;

			case UNKNOWN:
				if(option.debug >= 1)
					cout << "?" << x;
				break;
		}

        cout << ' ';
	}

	cout << endl;
}
