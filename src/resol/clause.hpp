#ifndef CLAUSE_HPP
#define CLAUSE_HPP

#include "option.hpp"

#include <vector>
#include <list>
#include <ostream>
#include <stack>

#define DEBUG(X) if(option.debug >= X)\
					os

enum State{TRUE, FALSE, UNKNOWN};

enum Res {NEW, NOTHING, ERROR, SUCCESS};
enum Choix {INFER, GUESS};


struct Decision_var
{
    Decision_var(int n_var, Choix n_choice) {var = n_var; choice = n_choice;} // pas un struct ?????? on peut créer un constructeur pour un struct, plus facile à initialiser

    int var;
    Choix choice;
};

class Clause
{
    public:
		Clause();
        Clause(std::list<int> n_vars);

		//Get
		int get(){return vars.front();};
		unsigned int size(){return vars.size();};
		std::list<int>& get_vars(){return vars;};

		//DPLL
		void print();
		State test(std::vector<State>& assignment);
        void get_up(std::vector<bool>& be_cancelled,std::ostream& os, Option& option);
		void get_up_all();
        int apply_modification(std::vector<State>& assignment,std::ostream& os, Option& option); //Renvoie true si la clause est validé par les modifs

	private:
		std::list<int> vars;
		std::stack<int> stack_delete;
};

struct Decision_cla{Clause clause;int var;Decision_cla(Clause n_clause,int n_var)
{clause = n_clause;var = n_var;}};

#endif // CLAUSE_HPP
