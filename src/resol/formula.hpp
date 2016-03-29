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
		void set_clauses_alive(std::list<Clause>);

		// Get
		unsigned int nb_variables(){return assignment.size() - 1;};

		int get_first_var() const; // choix par défaut
		int get_random_var() const;
		int get_moms_var() const;
		int get_dlis_var() const;

		// Debug
		void print_formula(ostream& os, const Option& option, bool true_name = false, unsigned int debug_lvl = 3); // check
		void print_assignment(const Option&, ostream&); // print

		// ? à trier
		void update_var(int& x, ostream& os, Option& option);
		State test(ostream& os,Option& option);
		void revive(ostream& os,  Option& option, std::vector<bool> be_cancelled = std::vector<bool>({false}));
		void supprTauto(ostream& os, Option& option);
		void apply_modification(int& t,ostream& os, Option& option);
		Res propagation_unitary(std::stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unitary_wl(std::stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unique_polarity(std::stack<Decision_var>& decisions, ostream& os, Option& option);

	private:
		//!!! Plus performant avec des pointeurs ?
		std::list<Clause> clauses_alive;
		std::vector<std::list<Clause>> tab_stack_delete; //Clause peut devenir Decision_cla si on veut plus d'info
		//On utilise des list pour un backtrack rapide

		Renaming renaming;

		std::vector<State> assignment; //  [NULL, x_1, ..., x_n] donc taille (n + 1)
		std::list<unsigned int> var_alive;
};

#endif // FORMULA_HPP
