#ifndef THEORY_HPP
#define THEORY_HPP

#include <vector>
#include <list>
#include <queue>

#include "../interface/real_value.hpp"
#include "union_find.hpp"
#include "global.hpp"

class Theory
{
public:
	Theory(unsigned int n);
	bool add_connection(Real_Value& rv);
	std::vector<Real_Value> learning(Real_Value rv);
	void backtrack(Real_Value rv);

private:
	Union_find uf;

	std::vector<std::list<unsigned int> > graph_of_equality;
	std::list<std::pair<unsigned int, unsigned int> > inequalities;
};

#endif // THEORY_HPP
