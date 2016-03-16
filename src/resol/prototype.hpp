#include <iostream>

using namespace std;

void dpll(Formula& f, ostream& os, Option& option);
bool backtrack(Formula& f, stack<Decision_var>& decisions, ostream& os, Option& option);
void pretreatment(Formula& f, ostream& os, Option& option);
Res update(Formula& f, stack<Decision_var>& decisions, int& x,ostream& os, Option& option);
int get_next_var(Formula& f, ostream& os, Option& option);
