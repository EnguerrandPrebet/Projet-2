#ifndef FORMULA_HPP
#define FORMULA_HPP

#include "clause.hpp"
#include "global.hpp"
#include "theory.hpp"

#include "../interface/renaming.hpp"

#include <ostream>
#include <stack>
#include <list>
#include <map>
#include <vector>

#define WL_SIGNAL_SAT 0 // code retour pour les heuristiques

using namespace std;

class Formula
{
	public:
		Renaming renaming;

		Formula();

		Formula(const Renaming&);

		// Set
		void set_clauses_alive(const std::list<Clause>&);
		void time(int x, int time){time_of_assign[x] = time;}; //CL

		// Get
		unsigned int nb_variables() const;

		State variable_assignment(unsigned int x) const;

		int get_first_var() const; // choix par défaut
		int get_random_var() const;
		int get_moms_var() const;
		int get_dlis_var() const;

		int get_vsids_var() const;

		// Debug
		void print_formula(bool true_name = false) const; // check
		void print_assignment() const; // print

		//CL
		int add_learned_clause(std::vector<int>& new_clause, int uip); // retour : date du backtrack

		// ? à trier
		void update_var(int l, Theory& theory);
		State check_satisfiability();
		void revive(const std::vector<bool>& be_cancelled = std::vector<bool>({false}));
		void pretreatment_remove_tautology();
		void apply_modification(int);
		Res propagation_unitary(std::stack<Decision>& decisions, Theory& theory);
		Res propagation_unique_polarity(std::stack<Decision>& decisions, Theory& theory);
		//WL
		Res propagation_unitary_wl(std::stack<Decision>& decisions, Theory& theory);


	private:
		std::list<Clause> clauses_alive;
		std::vector< std::list<Clause> > tab_stack_delete; //Clause peut devenir Decision_cla si on veut plus d'info

		std::vector<State> assignment; //  [NULL, x_1, ..., x_n] donc taille (n + 1)
		std::vector<int> time_of_assign; //CL

		/* <VSIDS */
		std::vector<float> vsids_literal_score; // x -> x,  (-x) -> n+x
		/* VSIDS> */


		std::list<unsigned int> var_alive;
};

#endif // FORMULA_HPP
