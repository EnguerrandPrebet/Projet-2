#ifndef CLAUSE_HPP
#define CLAUSE_HPP

#include <vector>
#include <list>
#include <stack>

enum State{TRUE, FALSE, UNKNOWN};

enum Res {NEW, NOTHING, ERROR, SUCCESS};
enum Choice {INFER, GUESS};




class Clause
{
	public:
		Clause();
		Clause(const std::list<int>&);

		//Get
		std::stack<int>& get_stack() {return stack_delete;};
		std::vector<int>& get_lit_fixed() {return literals_fixed;};

		int get_first_litteral() const {return literals_dyn.front();};
		unsigned int size() const {return literals_dyn.size();};
		std::list<int>& get_vars() {return literals_dyn;};
		const std::list<int>& get_vars() const {return literals_dyn;};

		//CL + WL
		int time_max(std::vector<int> time_of_assign);

		//DPLL
		void print() const;
		State check_satisfiability(const std::vector<State>& assignment);

		void get_up(const std::vector<bool>& be_cancelled);
		void get_up_wl();

		State litteral_status(const std::vector<State>& assignment, int l) const;
		int apply_modification(const std::vector<State>& assignment); //Renvoie true si la clause est validé par les modifs
		int apply_modification_wl(const std::vector<State>& assignment); //Renvoie true si la clause est validé par les modifs
		Res propagation_unitary_wl(const std::vector<State>& assignment, int& x);

	private:
		// option watched litterals
		std::vector<int> literals_fixed;
		int wl1, wl2;
		bool nothing_before_wl;
		bool nothing_after_wl;

		// non watched litterals
		std::list<int> literals_dyn;
		std::stack<int> stack_delete;
};

struct Decision_var
{
	int var;
	int time;
	Clause reason;
	Choice choice;
};

#endif // CLAUSE_HPP
