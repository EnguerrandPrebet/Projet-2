#include "formula_input.hpp"

#include "../tseitin/tseitin.hpp"

#include <sstream>

using namespace std;

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

int Formula_input::rename_litteral(int l)
{
	unsigned int x = abs(l);

	/* Variable déjà vue, déjà mappée */
	if (variables_mapping.find(x) != variables_mapping.end())
		return variables_mapping[x];

	/* Renommage */
	int mapped_x = variables_mapping[x] = Formula_input::next_available_var++;
	nb_input_variables++;

	return sign(l) * mapped_x;
}

int Formula_input::new_variable()
{
	int mapped_x = next_available_var++;

	/* On cherhce le prochain nom de variable disponible (input) */
	//!!!! pas efficace, code temporaire
	unsigned int x;
	for (x = 1; variables_mapping.find(x) != variables_mapping.end(); x++);
	variables_mapping[x] = mapped_x;

	return mapped_x;
}

unsigned int Formula_input::next_available_var = 1;

map<int, unsigned int> Formula_input::variables_mapping;

unsigned int Formula_input::nb_input_variables = 0;

/***********************************/
/* Variables */
/***********************************/
FVar_input::FVar_input(int new_l) :
	input_l(new_l)
{
	tseitin_x = Formula_input::rename_litteral(new_l);
}

std::string FVar_input::to_string() const
{
	stringstream ss;
	ss << input_l;
	return ss.str();
}

void FVar_input::tseitin_one_step(std::stack<Formula_input*>& jobs, list<Clause>& out)
{
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

	else
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

void FOperation_input::tseitin_one_step(stack<Formula_input*>& jobs, list<Clause>& out)
{
	switch(t)
	{
			case OR:
			{
				int i2 = Formula_input::new_variable();
				int i3 = Formula_input::new_variable();

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
				int i2 = Formula_input::new_variable();
				int i3 = Formula_input::new_variable();

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
				int i2 = Formula_input::new_variable();
				int i3 = Formula_input::new_variable();

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
				int i2 = Formula_input::new_variable();
				int i3 = Formula_input::new_variable();

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
				int i2 = Formula_input::new_variable();
				int i3 = Formula_input::new_variable();

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
				int i2 = Formula_input::new_variable();

				set<int> c1({tseitin_x, i2});
				set<int> c2({-tseitin_x, -i2});
				out.push_back(list<int>(c1.begin(), c1.end()));
				out.push_back(list<int>(c2.begin(), c2.end()));

				l->tseitin_x = i2;
				jobs.push(l);

			}   break;

	}
}
