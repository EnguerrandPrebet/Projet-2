#ifndef FORMULA_HPP
#define FORMULA_HPP

#include "clause.hpp"
#include "option.hpp"

#include "renaming.hpp"

#include <ostream>
#include <stack>
#include <list>
#include <map>
#include <vector>

using namespace std;

class Formula
{
	public:
		Formula();

		Formula(const Renaming&);

		// Set
		void set_clauses_alive(const std::list<Clause>&);
		void time(const int& x, const int& time){time_of_assign[x] = time;}; //CL

		// Get
		unsigned int nb_variables() const;

		int get_first_var() const; // choix par défaut
		int get_random_var() const;
		int get_moms_var() const;
		int get_dlis_var() const;

		// Debug
		void print_formula(ostream& os, const Option& option, const bool& true_name = false, const unsigned int& debug_lvl = 3) const; // check
		void print_assignment(const Option&, ostream&) const; // print

		//CL
		int generate_new_clause(std::vector<int>& new_clause, const int& uip, Clause& clause_learned);
		bool comp(int i, int j){return time_of_assign[i] < time_of_assign[j];}; ///?à passer en private ? car elle n'a aucune raison d'être accessible dehors

		// ? à trier
		void update_var(const int& l, ostream& os, const Option&);
		State check_satisfiability(ostream& os, const Option&);
		void revive(ostream& os,  const Option&, const std::vector<bool>& be_cancelled = std::vector<bool>({false}));
		void remove_tautology(ostream& os, const Option&);
		void apply_modification(const int& t,ostream& os, const Option&);
		Res propagation_unitary(std::stack<Decision_var>& decisions, ostream& os, const Option&);
		Res propagation_unique_polarity(std::stack<Decision_var>& decisions, ostream& os, const Option&);
		//WL
		Res propagation_unitary_wl(std::stack<Decision_var>& decisions, ostream& os, const Option&);

	private:
		std::list<Clause> clauses_alive;
		std::vector< std::list<Clause> > tab_stack_delete; //Clause peut devenir Decision_cla si on veut plus d'info

		Renaming renaming;

		std::vector<State> assignment; //  [NULL, x_1, ..., x_n] donc taille (n + 1)
		std::vector<int> time_of_assign; //CL


		//std::vector< std::list<int> > reason_of_assignment; // clause learning useless

		std::list<unsigned int> var_alive;
};

#endif // FORMULA_HPP
