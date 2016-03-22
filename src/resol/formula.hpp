#ifndef FORMULA_HPP
#define FORMULA_HPP

#include "clause.hpp"
#include "option.hpp"

#include <ostream>
#include <stack>
#include <list>
#include <map>

#define INPUT_NAMES false
#define INTERNAL_NAMES true

using namespace std;

class Formula
{
	public:
		Formula();

		Formula(std::map<int, unsigned int> variables_mapping); // Les clés vont de 1 à variables_mapping.size()

		// Set
		void set_clauses_alive(std::list<Clause>);

		// Get
		unsigned int get_nb_Var(){return nb_Var;};
		int get_first_var() const; // choix par défaut
		int get_random_var() const;
		int get_moms_var() const;

		// Debug
		void print_formula(ostream& os, const Option& option, bool true_name = false, unsigned int debug_lvl = 3); // check
		void print_assignment(const Option& option, ostream& os); // print

		// ? à trier
		void update_var(int& x, ostream& os, Option& option);
		State test(ostream& os,Option& option);
		void revive(ostream& os,  Option& option, std::vector<bool> be_cancelled = std::vector<bool>({false}));
		void supprTauto(ostream& os, Option& option);
		void apply_modification(int& x,ostream& os, Option& option);
		Res propagation_unitary(std::stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unitary_wl(std::stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unique_polarity(std::stack<Decision_var>& decisions, ostream& os, Option& option);

	private:
		unsigned int nb_Var; //Nb de variable (assignment [NULL,x1,...,xn])

		std::list<Clause> clauses_alive;
		std::vector<std::list<Clause>> tab_stack_delete; //Clause peut devenir Decision_cla si on veut plus d'info
		//On utilise des list pour un backtrack rapide

		std::map<int, unsigned int> var_true_name;
		std::vector<State> assignment;
		std::list<unsigned int> var_alive;
};

#endif // FORMULA_HPP
