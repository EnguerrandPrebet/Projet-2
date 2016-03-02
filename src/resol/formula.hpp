#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <vector>
#include <set>
#include <iostream>


class Formula
{
    public:
        Formula();
        Formula(unsigned int v);

        unsigned int v() { return _v; }

        unsigned int c() { return clauses.size(); }

			void set_v(unsigned int v) { _v = v;}
        std::vector<std::set<int>> clauses;

		friend std::ostream& operator<<(std::ostream& os, const Formula& f);

    private:
        unsigned int _v;//Nombre de variables

};

#endif // FORMULA_HPP
