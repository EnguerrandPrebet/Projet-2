#include "option.hpp"

#include <iostream>

using namespace std;

void layered_debug(Option& option, ostream& os, const string& s, unsigned int X)
{
	if(option.debug >= X)
		os << s;
}
