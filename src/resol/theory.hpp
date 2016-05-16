#ifndef THEORY_HPP
#define THEORY_HPP

#include <vector>
#include <list>
#include <queue>

#include "../tseitin/truc.hpp"
#include "union_find.hpp"
#include "global.hpp"

class Theory
{
public:
	Theory(unsigned int n);

	bool add_connection(Real_Value&);
	std::vector<Real_Value> learning(Real_Value);
	void backtrack(Real_Value);

private:
	Union_find uf;

	std::vector<std::list<unsigned int> > graph_of_equality;
	std::list<std::pair<unsigned int, unsigned int> > inequalities;
};

#endif // THEORY_HPP
