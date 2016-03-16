#include "formula.hpp"

#include <iostream>

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
}
