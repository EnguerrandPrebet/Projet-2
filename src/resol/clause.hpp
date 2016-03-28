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
    Decision_var(int n_var, Choix n_choice, int n_time) {var = n_var; choice = n_choice; time = n_time;} //! Clem à gérer

    int var;
    int time = 0;
    Choix choice;
};

class Clause
{
    public:
		Clause();
		Clause(std::list<int>);

		//Get
		int get(){return literals_dyn.front();};
		unsigned int size() const {return literals_dyn.size();};
		std::list<int>& get_vars() {return literals_dyn;};
		const std::list<int>& get_vars() const {return literals_dyn;};

		//DPLL
		void print();
		State test(std::vector<State>& assignment);
        void get_up(std::vector<bool>& be_cancelled,std::ostream& os, Option& option);
		void get_up_wl();
		void get_up_all();
        State litteral_status(std::vector<State>& assignment, int& x);
        int apply_modification(std::vector<State>& assignment,std::ostream& os, Option& option); //Renvoie true si la clause est validé par les modifs
		int apply_modification_wl(std::vector<State>& assignment,std::ostream& os, Option& option); //Renvoie true si la clause est validé par les modifs
		Res propagation_unitary_wl(std::vector<State>& assignment, std::ostream& os, Option& option, int& x);

	private:
		// option watched litterals
		std::vector<int> literals_fixed;
		unsigned int wl1, wl2;
		bool nothing_before_wl;
		bool nothing_after_wl;

		// non watched litterals
		std::list<int> literals_dyn;
		std::stack<int> stack_delete;
};

struct Decision_cla
{
	Clause clause;
	int var;

	Decision_cla(Clause n_clause,int n_var)
	{
		clause = n_clause;
		var = n_var;
	}
};

#endif // CLAUSE_HPP
