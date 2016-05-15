#ifndef CLAUSE_HPP
#define CLAUSE_HPP

#include <vector>
#include <list>
#include <stack>

enum State{FALSE = 0, TRUE = 1, UNKNOWN}; // pour une conversion facile

enum Res {NEW, NOTHING, ERROR, SUCCESS};
enum Choice {INFER, GUESS};


/* gestion dynamique, stocke une "clause" et permet de savoir si elle est satisfaite,
 * évolue au cours des assignations, permet de backtracker.
*/
class Clause
{
	public:
		Clause();
		Clause(const std::list<int>&);

		//Get
		std::stack<int>& get_stack();
		int get_first_litteral() const;

		std::vector<int>& get_lit_fixed();

		// dans le cas wl on doit donner assignment
		unsigned int size(const std::vector<State>& assignment = std::vector<State>()) const; // nombre de variables UNKNOWN dans la clause

		std::list<int> get_vars_dyn() const;
		// dans le cas wl on doit donner assignment
		std::list<int> get_vars(const std::vector<State>& assignment = std::vector<State>()) const;

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

struct Decision
{
	int var;
	int time;
	Clause reason;
	Choice choice;
};

#endif // CLAUSE_HPP
