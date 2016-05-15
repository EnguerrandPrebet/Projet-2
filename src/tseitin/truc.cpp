#include "truc.hpp"

#include <iostream>


using namespace std;

template <typename T> int sign(const T& val) {
	return (T(0) < val) - (val < T(0));
}

int sign(Eq eq)
{
	return (eq.equal) - (!eq.equal);
}

Eq abs(Eq eq)
{
	Eq output = eq;
	output.equal = true;
	return output;
}

bool operator<(Eq const& eq1, Eq const& eq2)
{
	if(eq1.n1 < eq2.n1)
		return true;
	if(eq1.n1 > eq2.n1)
		return false;
	else
		return (eq1.n2 < eq2.n2);
}

Real_Value::Real_Value(int l)
{
	val.l = l;
	isint = true;
}

Real_Value::Real_Value(Eq eq)
{
	val.eq = eq;
	isint = false;
}

Eq Real_Value::g()
{
	return val.eq;
}

ostream& operator<<(ostream &os, Eq const& eq)
{
	os << eq.n1;
	if(eq.equal)
		os << " = ";
	else
		os << " != ";
	os << eq.n2;

	return os;
}

ostream& operator<<(ostream &os, Real_Value const& rv)
{
	if(rv.isint)
		os << rv.val.l;
	else
		os << rv.val.eq;

	return os;
}

Real_Value operator*(int sign, Real_Value rv)
{
	Real_Value output = rv;
	if(rv.isint)
	{
		output.val.l *= sign;
	}
	else
	{
		if(output.val.eq.equal)
			output.val.eq.equal = (sign == 1);
		else
			output.val.eq.equal = (sign == -1);
	}

	return output;
}

bool operator<(Real_Value const& rv1, Real_Value const& rv2)
{
	if(rv1.isint) //On suppose que toutes les valeurs stockées seront du même type
		return rv1.val.l < rv2.val.l;
	else
		return rv1.val.eq < rv2.val.eq;
}

Real_Value abs(Real_Value rv)
{
	Real_Value output = rv;
	if(rv.isint)
		output.val.l = abs(output.val.l);
	else
		output.val.eq = abs(output.val.eq);

	return output;
}

int sign(Real_Value rv)
{
	if(rv.isint)
		return sign(rv.val.l);
	else
		return sign(rv.val.eq);
}

unsigned int max(unsigned int i, Real_Value rv)
{
	if(rv.isint)
		return 0; //Inutile
	else
		return max(i,max(rv.val.eq.n1,rv.val.eq.n2));
}
