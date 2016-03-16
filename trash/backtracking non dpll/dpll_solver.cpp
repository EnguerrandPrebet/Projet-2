#include "dpll_solver.hpp"

#include "dpll_solver_utils.hpp"

#include <vector>
#include <stack>
#include <iterator>

using namespace std;

pair< choice, vector<choice> > dpll_solver(Formula f, ostream& os, int debug)
{
	if (debug>1)
	{
		os << "Formula before pretreatment: " << endl;
		os << f << endl << endl;
	}

    vector<choice> assignments(f.v()+1, UNKNOWN); //  X_0, ..., X_{v-1}

    f = pretreatment(assignments,f,os,debug);

    if(debug>1)
		os << endl << "After pretreatment: " << endl << f << endl << endl;

    stack<Decision> decisions;

    unsigned int k = 0;
    unsigned int i = 0;

    set<int>::iterator it;
    if(f.c() != 0)
        it = f.clauses[k].begin();

    /* Invariants : les clauses C_0, ..., C_{k-1} sont satisfaites, */

    while (k < f.c()) //    C_0 /\ ... /\ C_{c-1}
    {
        if (i >= f.clauses[k].size())
        {
            if (debug>2)
                os << "backtracking : C_" << k+1 << " unsatifiable." << endl;

            while (!decisions.empty())
            {
                const Decision decision = decisions.top();
                decisions.pop();
                if (decision.t == Decision::GUESS)
                {
					i = decision.i;
					k = decision.k;
					it = f.clauses[k].begin();
                    advance(it, i);

					choice new_value = not_choice(decision.value);
					assignments[decision.x] = new_value;

                    if (debug>2)
                        os << "return at C_" << k+1 << " " << i << endl;

					decisions.push({Decision::INFER, i, k, decision.x, new_value});
					break;
				}
                else /* (decision.t == INFER) on continue de dépiler */
                {
					assignments[decision.x] = UNKNOWN;
                }
            }

            if (decisions.empty())
				return make_pair(FALSE, assignments);

			continue;
        }

        int l_i = *it;

        if (truth_value(l_i, assignments) == TRUE)
        {
            k++;
            i = 0;
            it = f.clauses[k].begin();
        }
        else if (truth_value(l_i, assignments) == FALSE)
        {
            i++;
            it++;
        }
        else /* (truth_value(l_i) == UNKNOWN) */
        {
            choice value = satisfy_litteral(l_i);

            if (debug>2)
                os << "push GUESS : " << abs(l_i) << " = " << value << "  at C_" << k+1 << " " << i << endl;

            assignments[abs(l_i)] = value;
            decisions.push({Decision::GUESS, i, k, (unsigned int)abs(l_i), value});
            k++;
            i = 0;
            it = f.clauses[k].begin();
        }
    }

    return make_pair(TRUE, assignments);
}
