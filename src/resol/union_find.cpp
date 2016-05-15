#include "union_find.hpp"

#include <cassert>

using namespace std;

Union_find::Union_find(UF::type t, unsigned int n) : type(t), weight(n, 1), father(n)
{
	for (unsigned int i = 0; i < n; i++)
		father[i] = i;
}

void Union_find::merge(unsigned int i, unsigned int j)
{
	unsigned int p_i = rep(i);
	unsigned int p_j = rep(j);

	if (p_i == p_j)
		return;

	if (weight[p_i] < weight[p_j])
	{
		swap(i, j);
		swap(p_i, p_j);
	}

	/* Maintenant w[p_i] >= w[p_j] */
	if (type == UF::BACKTRACKING)
	{
		backtrack.push({p_i, p_j, weight[p_j]});
	}

	father[p_j] = p_i;
	weight[p_i] += weight[p_j];
}

unsigned int Union_find::rep(unsigned int i) const
{
	// i = father[father...[i]] // heuristique ?

	if (father[i] == i)
		return i;
	else
		return rep(father[i]);
}

bool Union_find::equals(unsigned int i, unsigned int j) const
{
	return rep(i) == rep(j);
}

void Union_find::one_step_back()
{
	assert(type == UF::BACKTRACKING);

	const Step& _ = backtrack.top();

	father[_.p_j] = _.p_j;
	weight[_.p_i] -= _.w_j;
	weight[_.p_j] = _.w_j;

	backtrack.pop();
}
