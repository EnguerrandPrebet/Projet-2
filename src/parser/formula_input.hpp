#ifndef DEF_FORMULA_INPUT_HPP
#define DEF_FORMULA_INPUT_HPP

#include "../resol/formula.hpp"

#include <string>
#include <set> //???????? encore utile
#include <stack>
#include <vector>
#include <map>

/***********************************/
/* Formules */
/***********************************/
class Formula_input
{
public:
	Formula_input(){ tseitin_x = 0; } // pour tseitin

	virtual std::string to_string() const = 0;

	//int new_
	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out){}

	static int rename_litteral(int l);
	static int new_variable();

	int tseitin_x; // pour tseitin

	/* Renaming of variables */
	static unsigned int next_available_var;
	static std::map<int, unsigned int> variables_mapping;
	static unsigned int nb_input_variables;
};

/***********************************/
/* Variables */
/***********************************/
class FVar_input : public Formula_input
{
public:
	FVar_input(int);

	virtual std::string to_string() const;

	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out);

	int input_l;
};

/***********************************/
/* Opération de Formules */
/***********************************/
class FOperation_input : public Formula_input
{
public:
	enum OpType {AND, OR, XOR, IMPLY, EQUIV, NEGATE};

	FOperation_input(Formula_input* new_l, Formula_input* new_r, enum OpType new_t);

	virtual std::string to_string() const;

	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out);

	OpType t;
	Formula_input* l,* r; // dans le cas où t == NEGATE, r vaut NULL
};

#endif // ! DEF_FORMULA_INPUT_HPP
