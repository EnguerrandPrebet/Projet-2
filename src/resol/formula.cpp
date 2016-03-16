#include "formula.hpp"

#include <iostream>
#include <algorithm>
using namespace std;

Formula::Formula()
{

}
Formula::Formula(map<unsigned int,int> n_vars)
{
	var_true_name = n_vars;
	nb_Var = n_vars.size();
	assignment = vector<State>(nb_Var,UNKNOWN);
}

void Formula::update_var(int& x,ostream& os,Option& option)
{
	if(x>0)
		assignment[x] = TRUE;
	else
		assignment[-x] = FALSE;

	var_alive.erase(find(var_alive.begin(),var_alive.end(),abs(x)));
}

void Formula::apply_modification(int& x,ostream& os,Option& option)
{
	for(auto it = clauses_alive.begin(); it != clauses_alive.end(); it++)
	{
		if(it->apply_modification(x,os,option))
		{
			stack_delete.push(Decision_cla(*it,x));
			clauses_alive.erase(it);
		}

	}
}
