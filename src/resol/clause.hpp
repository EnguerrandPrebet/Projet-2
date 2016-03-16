#ifndef CLAUSE_HPP
#define CLAUSE_HPP


#define NO_DEBUG 0

#include <vector>
#include <list>

#define abs(X) (X>0)?X:-X
using namespace std;

enum State{TRUE,FALSE,UNKNOWN};

enum Res {NEW,NOTHING,ERROR,SUCCESS};
enum Choix {INFER,GUESS};

enum Heuristique {NONE};

struct Option
{
	bool tseitin = false;
	int debug = NO_DEBUG;
	bool cnf_found = false;
	bool for_found = false;
	bool lw = false;
	Heuristique get = NONE;
};

class Clause
{
	public:
		Clause();
		int get(){return vars.front();};
		unsigned int size(){return vars.size();};
		State test(vector<State>& assignment);
		void get_up(int var = 0);
		list<int> get_vars(){return vars;};
	private:
		list<int> vars;
};

struct Decision_var{int var;Choix choice;Decision(int n_var, Choix n_choice)
{var = n_var; choice = n_choice;}};
struct Decision_cla{Choix choice;Clause clause;Decision(Choix n_choice,Clause n_clause  = Clause())
{choice = n_choice;clause = n_clause}};

#endif // CLAUSE_HPP
