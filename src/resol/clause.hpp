#ifndef CLAUSE_HPP
#define CLAUSE_HPP


#define NO_DEBUG 0

#include <vector>
#include <list>
#include <iostream>
#include <stack>

#define abs(X) (X>0)?X:-X
#define DEBUG(X) if(option.debug >= X)\
					os
using namespace std;

enum State{TRUE,FALSE,UNKNOWN};

enum Res {NEW,NOTHING,ERROR,SUCCESS};
enum Choix {INFER,GUESS};

enum Heuristique {NONE};


struct Decision_var{int var;Choix choice;Decision_var(int n_var, Choix n_choice)
{var = n_var; choice = n_choice;}};

struct Option
{
	bool tseitin = false;
	int debug = NO_DEBUG;
	bool cnf_found = false;
	bool for_found = false;
	bool lw = false;
	Heuristique get = NONE;
	bool redirect_os = false;
};

class Clause
{
	public:
		Clause();
		Clause(list<int> n_vars){vars = n_vars;};
		void print();
		int get(){return vars.front();};
		unsigned int size(){return vars.size();};
		State test(vector<State>& assignment);
		void get_up(vector<bool>& be_cancelled,ostream& os,Option& option);
		void get_up_all();
		list<int>& get_vars(){return vars;};
		int apply_modification(vector<State>& assignment,ostream& os,Option& option); //Renvoie true si la clause est validé par les modifs
	private:
		list<int> vars;
		stack<int> stack_delete;
};

struct Decision_cla{Clause clause;int var;Decision_cla(Clause n_clause,int n_var)
{clause = n_clause;var = n_var;}};

#endif // CLAUSE_HPP
