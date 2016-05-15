#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

#include <vector>
#include <stack>
#include <utility>

namespace UF
{
	enum type {NO_BACKTRACKING, BACKTRACKING};
}

class Union_find // gère la fusion avec poids mais pas la compression de chemin
{
public:
	Union_find(UF::type, unsigned int n);

	void merge(unsigned int, unsigned int);

	unsigned int rep(unsigned int) const;

	bool equals(unsigned int, unsigned int) const;

	/* annule le dernier appel à merge(i, j) qui a eu une effet */ //!!! NON ! Même celles inutiles doivent être comptées (DPLL ne fait pas la différence, et ne sait pas combien de fois il doit l'appeler sinon)
	void one_step_back(); // l'appeler seulement si BACKTRACKING est activé


private:
	struct Step
	{
		unsigned int p_i; // plus gros
		unsigned int p_j; // plus petit
		unsigned int w_j; // ancien poids de p_j
	};

private:
	const UF::type type;

	std::stack< Step > backtrack;
	std::vector<unsigned int> weight;
	std::vector<unsigned int> father;
};

#endif // UNION_FIND_HPP
