#include "formula.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include <climits> // pour initialisation des min / max
#include <cassert> // pour s'assurer que la formule ne contient pas de clause UNKNOWN

#define VSIDS_DECAY_FACTOR 2
#define VSIDS_ACTIVITY_INCREASE_ADD 1

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

	tab_stack_delete = vector< list<Clause> >(n + 1);

	if (Global::option.heuristic == VSIDS)
		vsids_literal_score = vector<float>(2*n + 1, 0.0); // 0 inutilisé + n fois x + n fois (-x)
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
	{
		unsigned int clause_size = clause.size(assignment);
		if (clause_size != 0) // wl
			min_clause_size = min(min_clause_size, clause_size);
	}

	if (min_clause_size == UINT_MAX)
		return WL_SIGNAL_SAT;

	/* On cherche le littéral qui apparaît le plus souvent dans ces clauses */
	int max_l = 1; // initialisation sans intérêt, pour enlever le warning
	unsigned int max_freq = 0;

	map<int, unsigned int> frequencies;
	for (const Clause& clause : clauses_alive)
		if (clause.size(assignment) == min_clause_size)
			for (int l : clause.get_vars(assignment))
			{
				unsigned int new_freq_l = (++frequencies[l]); // si frequencies[x] n'est pas présent, il est initialisé à 0

				if (new_freq_l > max_freq)
				{
					max_l = l;
					max_freq = new_freq_l;
				}
			}

	return max_l;
}

int Formula::get_dlis_var() const
{
	/* On cherche l'assignation qui rend le plus de clauses valides */
	map<int, unsigned int> literals_score;

	for (const Clause& clause : clauses_alive)
		for (int l : clause.get_vars(assignment)) // chaque littéral doit apparaître de façon unique dans une clause
			literals_score[l]++; // 'l' satisfait 'clause'
			/* remarque : si variables_score[l] n'existe pas encore il est initialisé à 0 */

	/* On récupère le littéral de meilleur score */
	auto cmp =[](const pair<int, unsigned int>& p1, const pair<int, unsigned int>& p2) { return p1.second < p2.second; };
	const auto& max_score = max_element(literals_score.begin(), literals_score.end(), cmp);

	int max_score_l = max_score->first;

	return max_score_l;
}

int Formula::get_vsids_var() const
{
	int best_l = 0;
	unsigned int best_i = 0; // anti warning

	/* Pour chaque (x, -x) non assigné on cherche celui qui maximise le score vsids */
	unsigned int n = nb_variables();
	for (unsigned int x = 1; x <= n; x++)
	{
		if (assignment[x] == UNKNOWN)
		{
			unsigned int i = x;
			unsigned int j = n + x;

			/* initialisation non effectuée */
			if (best_l == 0)
			{
				// x est meilleur que (-x)
				if (vsids_literal_score[i] > vsids_literal_score[j])
				{
				best_l = x;
				best_i = i;
				}
				else // (-x) est meilleur que x
				{
					best_l = (-x);
					best_i = j;
				}
			}

			/* initialisation effectuée */
			else
			{
				// x est meilleur que best_l
				if (vsids_literal_score[i] > vsids_literal_score[best_i])
				{
					best_l = x;
					best_i = i;
				}
				// (-x) est meilleur que best_l
				else if (vsids_literal_score[j] > vsids_literal_score[best_i])
				{
					best_l = (-x);
					best_i = j;
				}
				// else aucune amélioration
			}
		}
	}

	return best_l;
}

void Formula::update_var(int l, Theory& theory)
{
	unsigned int x = abs(l);

	assignment[x] = (l > 0) ? TRUE : FALSE;

	Global::DEBUG(1) << x << " set to " << (l>0) << endl;

	auto it = find(var_alive.begin(), var_alive.end(), x);
	var_alive.erase(it);

	Global::DEBUG(1) << var_alive.size() << " vars left" << endl;


	if(Global::option.smt && renaming.is_input_variable(x))
	{
		Real_Value new_rv = renaming.inverse_translate_litteral(l);
		if(theory.add_connection(new_rv))
		{
			vector<Real_Value> smt_clause = theory.learning(new_rv);
			vector<int> new_clause;

			for(Real_Value rv:smt_clause)
			{
				Global::DEBUG(2) << rv << endl;
				new_clause.push_back(renaming.translate_litteral(rv));
			}

			add_learned_clause(new_clause,x);
		}

	}
}

void Formula::apply_modification(int t)
{
	Global::DEBUG(1) << "Modifying f" << endl;
	for(auto it = clauses_alive.begin(); it != clauses_alive.end();)
	{
		int cause;
		if ((Global::option.watched_litterals == false
			 && (cause = it->apply_modification(assignment)))
				|| (Global::option.watched_litterals == true && (cause = it->apply_modification_wl(assignment))))
		{
			Global::DEBUG(1) << "Clause deleted at t = " << t << ", because of " << cause <<  endl;
			tab_stack_delete[abs(cause)].push_back(*it);
			it = clauses_alive.erase(it);
			if(clauses_alive.empty())
			{
				Global::DEBUG(1) << "Il y a plus rien !" << endl;
				break;
			}
		}
		else
		{
			it++;
		}
	}
}

State Formula::check_satisfiability()
{
	revive(); // remet toutes les clauses vivantes

	State result = TRUE;
	/* On vérifie que toutes les clauses sont satisfaites */
	for (Clause& c : clauses_alive)
		switch (c.check_satisfiability(assignment))
		{
		case TRUE:
			continue;

		case FALSE:
			return FALSE;

		case UNKNOWN:
			result = UNKNOWN; // on renvoie UNKNOWN seulement si aucune clause n'est fausse
		}

	return result;
}

void Formula::pretreatment_remove_tautology()
{
	list<Clause> new_clauses({});
	for (Clause& c : clauses_alive)
	{
		c.check_satisfiability(assignment);
		map<int,bool> truc; //true : x, false : x bar
		bool tauto = false;
		for (int l : c.get_vars_dyn())
		{
			int x = abs(l);

			if (truc.find(x) != truc.end() && truc[x] == (l < 0)) // l'opposé est présent
			{
				tauto = true;
				break;
			}
			else
			{
				truc[x] = (l > 0);
			}
		}
		if (!tauto)
		{
			new_clauses.push_back(c);
		}
		else
		{
			Global::DEBUG(1) << "Tautologie found" << endl;
		}
	}

	set_clauses_alive(new_clauses);
}

Res Formula::propagation_unitary(stack<Decision>& decisions, Theory& theory)
{
	Res action = SUCCESS;

	for (auto it = clauses_alive.begin(); it != clauses_alive.end(); it++)
	{
		const Clause& c = *it;

		switch (c.size())
		{
			case 0:
			decisions.push({0, decisions.top().time, *it, INFER});//On push la clause vide pour l'avoir dans le clause learning
				//decisions.push(Decision_var(0, INFER, decisions.top().time, *it));//On push la clause vide pour l'avoir dans le clause learning
				return ERROR;

			case 1:
			{
				action = NEW;
				int l = c.get_first_litteral();
				Global::DEBUG(1) << "Unitaire avec : " << l;

				unsigned int x = abs(l);
				if(assignment[x] == UNKNOWN) // si une autre déduction de ce parcours ne l'a pas modifié
				{
					time_of_assign[x] = decisions.top().time;
					Global::DEBUG(1) << " à la date " << time_of_assign[x] << endl;
					update_var(l, theory);
					decisions.push({l, decisions.top().time, *it, INFER});
				}
				else
					Global::DEBUG(1) << endl;
				break;
			}

			default: /* c.size() >= 2 */
				if(action != NEW)
					action = NOTHING;
		}
	}

	return action;
}

Res Formula::propagation_unitary_wl(stack<Decision>& decisions, Theory& theory)
{
	Res act = SUCCESS;

	for(auto c = clauses_alive.begin(); c != clauses_alive.end(); c++)
	{
		int x;
		Res act_aux = c->propagation_unitary_wl(assignment, x);
		if(act_aux == NEW)
		{
			Global::DEBUG(1) << "Unitaire avec : " << x;
			if(assignment[abs(x)] == UNKNOWN) //Si une autre déduction de ce parcours ne l'a pas modifié
			{
				time_of_assign[abs(x)] = decisions.top().time;
				Global::DEBUG(1) << " à la date " << time_of_assign[abs(x)] << endl;
				update_var(x, theory);
				decisions.push({x, decisions.top().time, *c, INFER});
			}
			else
				Global::DEBUG(1) << endl;
			act = NEW;
		}
		else if(act_aux == ERROR)
		{
			decisions.push({0, decisions.top().time, *c, INFER});//On push la clause vide pour l'avoir dans le clause learning
			return ERROR;
		}

		if(act != NEW)
			act = NOTHING;
	}

	return act;
}

Res Formula::propagation_unique_polarity(stack<Decision>& decisions, Theory& theory)
{
	vector<int> seen(Formula::nb_variables()+1, 0); //0 : Nothing spotted, 1 : x spotted, -1 : x bar spotted, 2 : both spotted

	for (const Clause& c : clauses_alive)
	{
		for (int l : c.get_vars())
		{
			unsigned int x = abs(l);

			if (!seen[x])
			{
				seen[x] = l/x; // +/- 1
			}
			else if ((seen[x] < 0) != (l < 0)) //Different signs
			{
				seen[x] = 2;
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

			Global::DEBUG(1) << "Polarité unique avec : " << x << endl;
			if(assignment[abs(x)] == UNKNOWN)
			{
				update_var(x, theory);
				decisions.push({x, decisions.top().time, Clause(), INFER});
			}
		}
	}
	return action;
}

void Formula::revive(const vector<bool>& be_cancelled)
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
			if(Global::option.watched_litterals == false)
				it->get_up(be_cancelled);
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

State Formula::variable_assignment(unsigned int x) const
{
	return assignment[x];
}

void Formula::print_formula(bool true_name) const
{
	Global::DEBUG() << endl << "Check :" << endl;
	for (const Clause& clause : clauses_alive)
	{
		for(int l : clause.get_vars(assignment))
		{
			Global::DEBUG() << ((true_name) ? renaming.translate_litteral(l) : l) << ' ';
		}
		Global::DEBUG() << endl;
	}
	Global::DEBUG() << "END" << endl << endl;

}

void Formula::print_assignment() const
{
	for (const pair<Real_Value, unsigned int> _ : renaming)
	{
		Real_Value x = _.first;

		unsigned int mapped_x = _.second;

		switch (assignment[mapped_x])
		{
			case TRUE:
				Global::MSG() << x;
				break;

			case FALSE:
				Global::MSG() << (-1)*x;
				break;

			case UNKNOWN:
				if(Global::option.debug >= 1)
					Global::MSG() << "?";
				Global::MSG() << x; // affectation arbitraire
				break;
		}

		Global::MSG() << ' ';
	}

	Global::MSG() << endl;
}

int Formula::add_learned_clause(vector<int>& clause, int uip)
{
	auto cmp = [this](int i, int j) {return time_of_assign[i] < time_of_assign[j];};
	sort(clause.begin(),clause.end(), cmp); // le moins récent en premier

	list<int> signed_clause;

	int maxi = 0;
	for(int j: clause)
	{
		if(time_of_assign[j] > maxi)
			maxi = time_of_assign[j];

		int signed_j = (1 - 2*(assignment[j] == TRUE)) * j;
		signed_clause.push_back(signed_j);
	}

	int signed_uip = (1 - 2*(assignment[uip] == TRUE)) * uip;
	signed_clause.push_back(signed_uip);

	Clause new_clause(signed_clause);

	if (Global::option.heuristic == VSIDS)
	{
		/* diminution d'activité des variables */
		for (float& score : vsids_literal_score)
			score /= VSIDS_DECAY_FACTOR;


		/* la clause apprise augmente l'activitée */
		for (int l : new_clause.get_vars_dyn())
		{
			unsigned int x = abs(l);

			unsigned int offset = (l > 0) ? 0 : nb_variables();
			unsigned int i = x + offset;

			vsids_literal_score[i] += VSIDS_ACTIVITY_INCREASE_ADD;
		}
	}

	clauses_alive.push_back(new_clause);

	return maxi;
}
