#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <iostream>
#include <stack>
#include <list>
#include <map>

#include "clause.hpp"

#define NO_DEBUG 0

using namespace std;


class Formula
{
	public:
		void check(ostream& os,Option& option,bool true_name = false); //Debug

		Formula(map<int,unsigned int> n_vars);
		Formula();
		void update_var(int& x,ostream& os,Option& option);
		State test(ostream& os,Option& option);
		void revive(ostream& os,  Option& option, vector<bool> be_cancelled = vector<bool>({false}));
		void supprTauto(ostream& os, Option& option);
		void clear_c(list<Clause> clauses);
		void apply_modification(int& x,ostream& os, Option& option);
		Res propagation_unitary(stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unique_polarity(stack<Decision_var>& decisions, ostream& os, Option& option);
		void print(Option& option); //Affiche assignment
		int get_fst_var(){return var_alive.front();}; //Choix par défaut
		unsigned int get_nb_Var(){return nb_Var;};

	private:
		unsigned int nb_Var; //Nb de variable (assignment [NULL,x1,...,xn])

		list<Clause> clauses_alive;
		stack<Decision_cla> stack_delete;

		map<int,unsigned int> var_true_name;
		vector<State> assignment;
		list<unsigned int> var_alive;

};

#endif // FORMULA_HPP
