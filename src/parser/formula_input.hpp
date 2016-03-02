#ifndef DEF_FORMULA_INPUT_HPP
#define DEF_FORMULA_INPUT_HPP

#include "../resol/formula.hpp"

#include <string>
#include <set>
#include <stack>
#include <vector>

/***********************************/
/* Formules */
/***********************************/
class Formula_input
{
public:
	Formula_input(){ x = -1; } // pour tseitin
	virtual std::string to_string() = 0;

	virtual void iterate(std::set<int>& real_var, int& next_var,std::stack<Formula_input*>& pile,Formula& final_formula){}

    std::vector<int> var_appeared;
	int x;
};

/***********************************/
/* Variables */
/***********************************/
class FVar_input : public Formula_input
{
public:
	FVar_input(int new_x);
	virtual std::string to_string();
};

/***********************************/
/* Opération de Formules */
/***********************************/
class FOperation_input : public Formula_input
{
public:
	enum OpType {AND, OR, XOR, IMPLY, EQUIV, NEGATE};

	FOperation_input(Formula_input* new_l, Formula_input* new_r, enum OpType new_t);
	virtual std::string to_string();

    virtual void iterate(std::set<int>& real_var, int& next_var, std::stack<Formula_input*>& pile,Formula& final_formula);

	OpType t;
	Formula_input* l,* r; // dans le cas où t == NEGATE, r vaut NULL
};

#endif // ! DEF_FORMULA_INPUT_HPP
