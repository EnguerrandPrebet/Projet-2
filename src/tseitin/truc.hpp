#include <iostream>


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

	/*//Get et Set
	int g(int);//Les paramètres sont là pour différentier les fonctions
	Eq g(Eq);
	void s(int l);
	void s(Eq eq);*/
	bool is_int();

private:
	union Value val;
	bool isint;


friend std::ostream& operator<<(std::ostream &flux, Real_Value const& rv);
friend bool operator<(Real_Value const& rv1, Real_Value const& rv2);

friend Real_Value operator*(int sign, Real_Value rv);
friend Real_Value abs(Real_Value rv);
friend int sign(Real_Value rv);
};
