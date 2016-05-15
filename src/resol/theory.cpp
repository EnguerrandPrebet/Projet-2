#include "theory.hpp"

#include <vector>
#include <list>


using namespace std;

Theory::Theory(unsigned int n) : uf(UF::BACKTRACKING,n+1),graph_of_equality(n+1)
{
}

bool Theory::add_connection(Real_Value& rv) //Renvoie faise s'il y a une erreur
{
	Eq new_co = rv.g();
	Global::DEBUG(2) << rv << endl;
	if(new_co.equal)
	{
		uf.merge(new_co.n1,new_co.n2);

		Global::DEBUG(2) << new_co.n1 << " " << new_co.n2 << endl;
		graph_of_equality[new_co.n1].push_back(new_co.n2);
		graph_of_equality[new_co.n2].push_back(new_co.n1);

		for(pair<unsigned int, unsigned int> ineq : inequalities)
		{
			if(uf.equals(ineq.first, ineq.second))
			{
				rv = Real_Value({ineq.first, ineq.second, false});
				return true;
			}

		}
	}
	else
	{
		inequalities.push_back(make_pair(new_co.n1,new_co.n2));
		Global::DEBUG(2) << new_co.n1 << " " << new_co.n2 << endl;
		if(uf.equals(new_co.n1, new_co.n2))
			return true;
	}
	return false;
}

void bfs(int n1, int n2, const vector< list<unsigned int> >& la, vector<int>& pere)
{
	vector<bool> isSeen(la.size(),false);
	isSeen[n1] = true;
	queue<int> q({n1});

	while(!(q.empty()))
	{
		int challenger = q.front();
		isSeen[challenger] = true;
		q.pop();
		if(challenger == n2)
			return;

		for(int i:la[challenger])
		{
			Global::DEBUG(2) << i << " " << challenger << endl;

			if(isSeen[i] == false)
			{
				q.push(i);
				pere[i] = challenger;
				isSeen[i] = true;
			}
		}
	}
}
vector<Real_Value> Theory::learning(Real_Value rv)
{
	vector<int> pere(graph_of_equality.size(),-1);

	Eq new_co = rv.g();
	Global::DEBUG(1) << "Problem with " << rv << endl;
	bfs(new_co.n1,new_co.n2,graph_of_equality, pere);

	vector<Real_Value> output;

	Global::DEBUG(1) << "Clause learned by SMT : " << endl;
	unsigned int i = new_co.n2;
	Global::DEBUG(1) << new_co.n1 << " != " << new_co.n2 << endl;
	while(i != new_co.n1)
	{
		unsigned int aux = pere[i];
		output.push_back((Eq){min(i,aux),max(i,aux),true});

		Global::DEBUG(1) << " " << i << " = " << aux << endl;
		i = aux;
	}
	Global::DEBUG(1) << endl;
	return output;
}

void Theory::backtrack(Real_Value rv)
{
	uf.one_step_back();

	Eq new_co = rv.g();
	if(new_co.equal)
	{
		graph_of_equality[new_co.n1].erase(find(graph_of_equality[new_co.n1].begin(),graph_of_equality[new_co.n1].end(),new_co.n2));
		graph_of_equality[new_co.n2].erase(find(graph_of_equality[new_co.n2].begin(),graph_of_equality[new_co.n2].end(),new_co.n1));
	}
	else
	{
		inequalities.erase(find(inequalities.begin(),inequalities.end(),make_pair(new_co.n1,new_co.n2)));
	}

}
