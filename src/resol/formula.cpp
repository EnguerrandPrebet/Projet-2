#include "formula.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include <climits> // pour initialisation des min / max
#include <cassert> // pour s'assurer que la formule ne contient pas de clause UNKNOWN

using namespace std;

Formula::Formula()
{

}

Formula::Formula(const Renaming& input_renaming)
{
	renaming = input_renaming;

	unsigned int n = renaming.number_of_variables();

	for (unsigned int x = 1; x < n; x++)
		var_alive.push_back(x);

	assignment = vector<State>(n, UNKNOWN);
	time_of_assign = vector<int>(n, -1);

	//??? if (option.cl == true)
	//reason_of_assignment = vector< list<int> >(n + 1);

	tab_stack_delete = vector< list<Clause> >(n + 1);
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

	/* On génère | trouve sa valeur */
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

void Formula::update_var(const int& l, ostream& os, const Option& option)
{
	unsigned int x = abs(l);

	assignment[x] = (l > 0) ? TRUE : FALSE;

	DEBUG(1) << x << " set to " << (l>0) << endl;

	auto it = find(var_alive.begin(), var_alive.end(), x);
	var_alive.erase(it);

	DEBUG(1) << var_alive.size() << " vars left" << endl;
}

void Formula::apply_modification(const int& t,ostream& os, const Option& option)
{
	DEBUG(1) << "Modifying f" << endl;
	for(auto it = clauses_alive.begin(); it != clauses_alive.end();)
	{
		int cause;
		if ((option.watched_litterals == false
			 && (cause = it->apply_modification(assignment,os,option)))
				|| (option.watched_litterals == true && (cause = it->apply_modification_wl(assignment,os,option))))
		{
			DEBUG(1) << "Clause deleted at t = " << t << ", because of " << cause <<  endl;
			tab_stack_delete[abs(cause)].push_back(*it);
			it = clauses_alive.erase(it);
			if(clauses_alive.empty())
			{
				DEBUG(1) << "Il y a plus rien !" << endl;
				break;
			}
		}
		else
		{
			it++;
		}
	}
}

State Formula::check_satisfiability(ostream& os, const Option& option)
{
	revive(os,option); // remet toutes les clauses vivantes

	State result = TRUE;
	/* On vérifie que toutes les clauses sont satisfaites */
	for(Clause& c : clauses_alive)
		switch (c.check_satisfiability(assignment))
		{
		case TRUE:
			continue;

		case FALSE:
			return FALSE;

		case UNKNOWN:
			result = UNKNOWN; //On renvoie UNKNOWN seulement si aucune clause n'est fausse
		}

	return result;
}

void Formula::remove_tautology(ostream& os, const Option& option)
{
	list<Clause> new_clauses({});
	for(Clause& c : clauses_alive)
	{
		c.check_satisfiability(assignment);
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

Res Formula::propagation_unitary(stack<Decision_var>& decisions, ostream& os, const Option& option)
{
	Res action = SUCCESS;

	for (auto it = clauses_alive.begin(); it != clauses_alive.end(); it++)
	{
		const Clause& c = *it;

		switch(c.size())
		{
			case 0:
				decisions.push(Decision_var(0, INFER, decisions.top().time, *it));//On push la clause vide pour l'avoir dans le clause learning
				return ERROR;

			case 1:
			{
				action = NEW;
				int l = c.get_first_litteral();
				DEBUG(1) << "Unitaire avec : " << l << endl;

				unsigned int x = abs(l);
				if(assignment[x] == UNKNOWN) //Si une autre déduction de ce parcours ne l'a pas modifié
				{
					update_var(l, os, option);
					time_of_assign[x] = decisions.top().time;
					decisions.push(Decision_var(l, INFER, decisions.top().time, *it));
				}
				break;
			}

			default: // c.size() >= 2
				if(action != NEW)
					action = NOTHING;
		}
	}

	return action;
}

Res Formula::propagation_unitary_wl(stack<Decision_var>& decisions, ostream& os, const Option& option)
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
				decisions.push(Decision_var(x,INFER,decisions.top().time, *c));
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

Res Formula::propagation_unique_polarity(stack<Decision_var>& decisions, ostream& os, const Option& option)
{
	vector<int> seen(Formula::nb_variables()+1, 0); //0 : Nothing spotted, 1 : x spotted, -1 : x bar spotted, 2 : both spotted

	for(auto c = clauses_alive.begin(); c != clauses_alive.end(); c++)
	{
		for(auto j = c->get_vars().begin(); j!=c->get_vars().end(); j++)
		{
			if (!seen[abs(*j)])
			{
				seen[abs(*j)] = (*j)/(abs(*j)); // +/- 1
			}
			else if ((seen[abs(*j)] < 0) != ((*j) < 0)) //Different signs
			{
				seen[abs(*j)] = 2;
			}
		}
	}

	Res action = NOTHING;

	for(unsigned int i = 1; i <= Formula::nb_variables(); i++)
	{
		if(abs(seen[i]) == 1)
		{
			int x = i*seen[i];
			action = NEW;

			DEBUG(1) << "Polarité unique avec : " << x << endl;
			if(assignment[abs(x)] == UNKNOWN)
			{
				update_var(x, os, option);
				decisions.push(Decision_var(x, INFER, decisions.top().time, Clause()));
			}
		}
	}
	return action;
}

void Formula::revive(ostream& os,  const Option& option, const vector<bool>& be_cancelled)
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

void Formula::set_clauses_alive(const list<Clause>& clauses)
{
	clauses_alive = clauses;
}

unsigned int Formula::nb_variables() const
{
	return assignment.size() - 1;
}


void Formula::print_formula(ostream& os, const Option& option, const bool& true_name, const unsigned int& debug_lvl) const
{
	if(option.debug >= debug_lvl)
	{
		os << endl << "Check :" << endl;
		for (const Clause& clause : clauses_alive)
		{
			for(auto it = clause.get_vars().begin(); it != clause.get_vars().end(); it++)
			{
				os << ((true_name) ? renaming.translate_litteral(*it) : *it) << ' ';
			}
			os << endl;
		}
		os << "END" << endl << endl;
	}
}

void Formula::print_assignment(const Option& option, ostream& os) const
{
	for (const pair<int, unsigned int> _ : renaming)
	{
		int x = _.first;

		unsigned int mapped_x = _.second;

		switch (assignment[mapped_x])
		{
			case TRUE:
				os << x;
				break;

			case FALSE:
				os << (-x);
				break;

			case UNKNOWN:
				if(option.debug >= 1)
					cout << "?";
				cout << x; // affectation arbitraire
				break;
		}

		os << ' ';
	}

	os << endl;
}

int Formula::generate_new_clause(const list<int>& clause, const int& uip, Clause& clause_learned)
{
	int mini = time_of_assign[uip] + 1; //Plus grand que tout ce qu'il peut y avoir

	list<int> signed_clause;

	for(int j: clause)
	{
		if(time_of_assign[j] < mini)
			mini = time_of_assign[j];

		int signed_j = (2*(assignment[j] == TRUE) - 1) * j;
		signed_clause.push_back(signed_j);
	}

	int signed_uip = (2*(assignment[uip] == TRUE) - 1) * uip;
	signed_clause.push_back(signed_uip);

	Clause new_clause(signed_clause);
	clause_learned = new_clause;

	clauses_alive.push_back(new_clause);

	if(mini == time_of_assign[uip] + 1) //aka clause = (uip) -> déduction unitaire à t = 0
		mini = 0;
	return mini;
}
