#include "formula.hpp"

#include <iostream>
#include <algorithm>
using namespace std;

Formula::Formula()
{

}

void Formula::update_var(int& x,ostream& os,Option& option)
{
	if(x>0)
		assignment[x] = TRUE;
	else
		assignment[-x] = FALSE;

	var_alive.erase(find(var_alive.begin(),var_alive.end()));
}

void Formula::apply_modification(ostream& os,Option& option)
{
	for(auto it = clauses_alive.begin(); it != clauses_alive.end(); it++)
	{
		if(it->apply_modification(os,option))
			stack_delete.push(Decision())
	}
}
