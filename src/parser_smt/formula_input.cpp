#include "formula_input.hpp"

#include "../tseitin/tseitin.hpp"

#include <sstream>
#include <set> // pour supprimer les littéraux en doubles

using namespace std;

/***********************************/
/* Variables */
/***********************************/
FVar_input::FVar_input(unsigned int v1, unsigned int v2, bool equal) :
	rv({min(v1,v2),max(v1,v2),equal})
{
}

FVar_input::FVar_input(int l) : rv(l)
{
}

std::string FVar_input::to_string() const
{
	stringstream ss;
	ss << rv;
	return ss.str();
}

void FVar_input::compute_renaming(Renaming& renaming)
{
	tseitin_x = renaming.rename_litteral(rv);
}

/***********************************/
/* Opération de Formules */
/***********************************/
FOperation_input::FOperation_input(Formula_input* new_l, Formula_input* new_r, enum OpType new_t) :
	t(new_t),
	l(new_l),
	r(new_r)
{
	if (new_t == NEGATE)
	{
		r = NULL;
	}

}

std::string FOperation_input::to_string() const
{
	if (t == NEGATE)
	{
		return "(~" + l->to_string() + ")";
	}

	else /* Opération binaire */
	{
		string op_str;
		switch (t)
		{
		case AND:
			op_str = "/\\";
			break;
		case OR:
			op_str = "\\/";
			break;
		case XOR:
			op_str = "X";
			break;
		case IMPLY:
			op_str = "=>";
			break;
		case EQUIV:
			op_str = "<=>";
			break;
		default:;
		}

		return "(" + l->to_string() + op_str + r->to_string() + ")";
	}
}

void FOperation_input::compute_renaming(Renaming& renaming)
{
	l->compute_renaming(renaming);
	r->compute_renaming(renaming);
}

void FOperation_input::tseitin_one_step(stack<Formula_input*>& jobs, list<Clause>& out, Renaming& renaming)
{
	switch(t)
	{
			case OR:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();
				int i3 = (r->tseitin_x != 0)? r->tseitin_x : renaming.new_variable();

				set<int> c1({-tseitin_x, i2, i3});
				set<int> c2({tseitin_x, -i2});
				set<int> c3({tseitin_x, -i3});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));
				out.push_back(list<int>(c3.begin(), c3.end()));

				l->tseitin_x = i2;
				r->tseitin_x = i3;
				jobs.push(r);
				jobs.push(l);

			}   break;

			case AND:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();
				int i3 = (r->tseitin_x != 0)? r->tseitin_x : renaming.new_variable();

				set<int> c1({tseitin_x, -i2, -i3});
				set<int> c2({-tseitin_x, i2});
				set<int> c3({-tseitin_x, i3});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));
				out.push_back(list<int>(c3.begin(), c3.end()));

				l->tseitin_x = i2;
				r->tseitin_x = i3;
				jobs.push(r);
				jobs.push(l);

			}   break;

			case XOR:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();
				int i3 = (r->tseitin_x != 0)? r->tseitin_x : renaming.new_variable();

				set<int> c1({-tseitin_x, i2, i3});
				set<int> c2({-tseitin_x, -i2, -i3});
				set<int> c3({tseitin_x, i2, -i3});
				set<int> c4({tseitin_x, -i2, i3});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));
				out.push_back(list<int>(c3.begin(), c3.end()));
				out.push_back(list<int>(c4.begin(), c4.end()));

				l->tseitin_x = i2;
				r->tseitin_x = i3;
				jobs.push(r);
				jobs.push(l);

			}   break;

			case IMPLY:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();
				int i3 = (r->tseitin_x != 0)? r->tseitin_x : renaming.new_variable();

				set<int> c1({-tseitin_x, -i2, i3});
				set<int> c2({tseitin_x, i2});
				set<int> c3({tseitin_x, -i3});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));
				out.push_back(list<int>(c3.begin(), c3.end()));

				l->tseitin_x = i2;
				r->tseitin_x = i3;
				jobs.push(r);
				jobs.push(l);

			}   break;

			case EQUIV:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();
				int i3 = (r->tseitin_x != 0)? r->tseitin_x : renaming.new_variable();

				set<int> c1({tseitin_x, -i2, -i3});
				set<int> c2({tseitin_x, i2, i3});
				set<int> c3({-tseitin_x, -i2, i3});
				set<int> c4({-tseitin_x, i2, -i3});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));
				out.push_back(list<int>(c3.begin(), c3.end()));
				out.push_back(list<int>(c4.begin(), c4.end()));

				l->tseitin_x = i2;
				r->tseitin_x = i3;
				jobs.push(r);
				jobs.push(l);

			}   break;

			case NEGATE:
			{
				int i2 = (l->tseitin_x != 0)? l->tseitin_x : renaming.new_variable();

				set<int> c1({tseitin_x, i2});
				set<int> c2({-tseitin_x, -i2});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));

				l->tseitin_x = i2;
				jobs.push(l);

			}   break;

	}
}
