#include "formula_input.hpp"

#include "../tseitin/tseitin.hpp"

#include <sstream>

using namespace std;

/***********************************/
/* Variables */
/***********************************/
FVar_input::FVar_input(int new_x)
{
	x = new_x;
	var_appeared = {abs(x)};
}

std::string FVar_input::to_string()
{
    stringstream ss;
    ss << x;
    return ss.str();
}

/***********************************/
/* Opération de Formules */
/***********************************/
FOperation_input::FOperation_input(Formula_input* new_l, Formula_input* new_r, enum OpType new_t)
{
	t = new_t;
	l = new_l;
	r = new_r;

	if (new_t == NEGATE)
	{
		r = NULL;
		var_appeared = l->var_appeared;
	}
	else
	{
		var_appeared.resize(l->var_appeared.size()+r->var_appeared.size());
		var_appeared = l->var_appeared;
		var_appeared.insert(var_appeared.end(),r->var_appeared.begin(),r->var_appeared.end());
	}
}

std::string FOperation_input::to_string()
{
	if (t == NEGATE)
	{
		return "(~" + l->to_string() + ")";
	}

	else
	{
		string op_str;
		switch (t)
		{
		case AND:
			op_str = "/\\";
			break;
		case OR:
			op_str = "\\/";
			break;
		case XOR:
			op_str = "X";
			break;
		case IMPLY:
			op_str = "=>";
			break;
		case EQUIV:
			op_str = "<=>";
			break;
		default:;
		}

		return "(" + l->to_string() + op_str + r->to_string() + ")";
	}
}

void FOperation_input::iterate(set<int>& real_var, int& next_var, stack<Formula_input*>& pile,Formula& final_formula)
{
	switch(t)
        {
            case OR:
            {
                int i2 = get_next_var(real_var,next_var,*l), i3 = get_next_var(real_var,next_var,*r);

                set<int> c1({-x,i2,i3});
                set<int> c2({x,-i2});
                set<int> c3({x,-i3});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);
                final_formula.clauses.push_back(c3);

                l->x = i2;
                r->x = i3;
                pile.push(r);
                pile.push(l);

            }   break;

            case AND:
           {
                int i2 = get_next_var(real_var,next_var,*l), i3 = get_next_var(real_var,next_var,*r);

                set<int> c1({x,-i2,-i3});
                set<int> c2({-x,i2});
                set<int> c3({-x,i3});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);
                final_formula.clauses.push_back(c3);

                l->x = i2;
                r->x = i3;
                pile.push(r);
                pile.push(l);

            }   break;
            case XOR:
            {
                int i2 = get_next_var(real_var,next_var,*l), i3 = get_next_var(real_var,next_var,*r);

                set<int> c1({-x,i2,i3});
                set<int> c2({-x,-i2,-i3});
                set<int> c3({x,i2,-i3});
                set<int> c4({x,-i2,i3});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);
                final_formula.clauses.push_back(c3);
                final_formula.clauses.push_back(c4);

                l->x = i2;
                r->x = i3;
                pile.push(r);
                pile.push(l);

            }   break;
            case IMPLY:
            {
                int i2 = get_next_var(real_var,next_var,*l), i3 = get_next_var(real_var,next_var,*r);

                set<int> c1({-x,-i2,i3});
                set<int> c2({x,i2});
                set<int> c3({x,-i3});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);
                final_formula.clauses.push_back(c3);

                l->x = i2;
                r->x = i3;
                pile.push(r);
                pile.push(l);

            }   break;
            case EQUIV:
            {
                int i2 = get_next_var(real_var,next_var,*l), i3 = get_next_var(real_var,next_var,*r);

                set<int> c1({x,-i2,-i3});
                set<int> c2({x,i2,i3});
                set<int> c3({-x,-i2,i3});
                set<int> c4({-x,i2,-i3});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);
                final_formula.clauses.push_back(c3);
                final_formula.clauses.push_back(c4);

                l->x = i2;
                r->x = i3;
                pile.push(r);
                pile.push(l);

            }   break;
            case NEGATE:
            {
                int i2 = get_next_var(real_var,next_var,*l);

                set<int> c1({x,i2});
                set<int> c2({-x,-i2});
                final_formula.clauses.push_back(c1);
                final_formula.clauses.push_back(c2);

                l->x = i2;
                pile.push(l);

            }   break;

		}
}
