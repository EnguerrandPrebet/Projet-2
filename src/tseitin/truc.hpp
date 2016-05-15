#ifndef TRUC_HPP
#define TRUC_HPP

#include <iostream>
#include <algorithm>

struct Eq
{
	unsigned int n1;
	unsigned int n2;
	bool equal;
};

union Value
{
	Eq eq;
	int l;
};

class Real_Value
{
public:
	Real_Value();
	Real_Value(int l);
	Real_Value(Eq eq);

	//Get et Set
	Eq g();
	/*void s(int l);
	void s(Eq eq);*/
	bool is_int();

private:
	union Value val;
	bool isint;


friend std::ostream& operator<<(std::ostream &, Real_Value const&);
friend bool operator<(Real_Value const&, Real_Value const&);

friend unsigned int max(unsigned int i, Real_Value rv);
friend Real_Value operator*(int sign, Real_Value rv);
friend Real_Value abs(Real_Value rv);
friend int sign(Real_Value rv);
};

#endif // TRUC_HPP
