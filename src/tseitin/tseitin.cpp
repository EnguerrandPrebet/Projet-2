#include "tseitin.hpp"

#include "../parser/formula_input.hpp"
#include "../resol/formula.hpp"

#include <iostream>
#include <stack>
#include <vector>

using namespace std;

Formula tseitin(Formula_input& f_input,int debug, ostream& os)
{
    int next_var = 1;
    set<int> real_var(f_input.var_appeared.begin(),f_input.var_appeared.end());
	if(debug)
	{
		os << "Real variables in the formula: " << endl;
		for(auto var:real_var)
		{
			os << var << "  ";
		}
		os << endl << endl;
	}
	
    f_input.x = get_next_var(real_var,next_var,f_input);

    stack<Formula_input*> pile;
    pile.push(&f_input);
    Formula final_formula;


    while(!pile.empty())
    {
        Formula_input* aux = pile.top();
        pile.pop();
        aux->iterate(real_var,next_var,pile,final_formula);
    }

    set<int> c({f_input.x});
    final_formula.clauses.push_back(c);
    final_formula.set_v(next_var-1);
    return final_formula;
}

int get_next_var(set<int>& real_var,int& next_var,Formula_input& f_input)
{
    if(f_input.x == -1)
    {
        while(real_var.find(next_var)!=real_var.end())
        {
            next_var++;
        }
        next_var++;

        return next_var-1;
    }
    else
    {
        return f_input.x;
    }
}
