#include "formula.hpp"

#include <iostream>

using namespace std;

Formula::Formula()
{
	_v = 0;
}

Formula::Formula(unsigned int v)
{
    _v = v;
}


std::ostream& operator<<(std::ostream& os, const Formula& f)
{
	bool first_line = true;

	for(unsigned int i = 0; i < f.clauses.size(); i++)
    {
		if (!first_line)
			os << endl;
		else
			first_line = false;

        os << "C_" << i + 1 << " = ";
        for(auto l:f.clauses[i])
        {
            os << l << " ";
        }
    }

	return os;
}
