#ifndef DEF_FORMULA_INPUT_HPP
#define DEF_FORMULA_INPUT_HPP

#include "../resol/formula.hpp"
#include "../resol/renaming.hpp"

#include <string>
#include <stack>
#include <vector>

/***********************************/
/* Formules */
/***********************************/
class Formula_input
{
public:
	Formula_input(){ tseitin_x = 0; } // pour tseitin

	virtual std::string to_string() const = 0;

	virtual void compute_renaming(Renaming&) = 0;

	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out, Renaming&) = 0;

	int tseitin_x;
};

/***********************************/
/* Variables */
/***********************************/
class FVar_input : public Formula_input
{
public:
	FVar_input(unsigned int v1, unsigned int v2, bool equal);
	FVar_input(int l);

	virtual std::string to_string() const;

	virtual void compute_renaming(Renaming&);

	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out, Renaming&){}

	Real_Value rv;
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

	virtual void compute_renaming(Renaming&);

	virtual void tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out, Renaming&);

	OpType t;
	Formula_input* l,* r; // dans le cas où t == NEGATE, r vaut NULL
};

#endif // ! DEF_FORMULA_INPUT_HPP
