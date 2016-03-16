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
		Formula(map<unsigned int,int> n_vars);
		Formula();
		void update_var(int& x,ostream& os,Option& option);
		State test();
		void revive(int var = 0, ostream& os = cout, int debug = 0);
		void supprTauto(ostream& os, Option& option);
		void clear_c(list<Clause> clauses);
		void apply_modification(int& x,ostream& os, Option& option);
		Res propagation_unitary(stack<Decision_var>& decisions, ostream& os, Option& option);
		Res propagation_unique_polarity(stack<Decision_var>& decisions, ostream& os, Option& option);
		int get_fst_var(){return var_alive.front();}; //Choix par défaut

	private:
		unsigned int nb_Var; //Nb de variable (assignment [NULL,x1,...,xn])

		list<Clause> clauses_alive;
		stack<Decision_cla> stack_delete;

		map<unsigned int,int> var_true_name;
		vector<State> assignment;
		list<unsigned int> var_alive;

};

#endif // FORMULA_HPP
