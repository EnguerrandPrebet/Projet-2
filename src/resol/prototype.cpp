#include <iostream>

using namespace std;

enum Res {NEW,NOTHING,ERROR,SUCCESS};

void dpll(Formula& f, ostream& os, Opt& option)
{

	pretreatment(f,os,option);

	stack<Decision> decisions({});

	bool sat_unk = true;
	while(sat_unk)
	{
		Res action_result;

		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f,decisions,os,option);
		}while(action_result==NEW)

		switch(action_result)
		{
			case ERROR:
				sat_unk = backtrack(f,decisions,os,option);
				break;
			case SUCCESS:
				sat_unk = false;
				break;
			case NOTHING: //default
				x = get_next_var(f,os,option); //x = littéral
				f.update_var(x,os,option); // met à jour en même temps assignment
				decisions.push((x,GUESS));
		}



	}

	//Peut être hors de la fonction DPLL (même si je ne vois pas pourquoi)
	switch(f.test())
	{
		case TRUE:
			cout << "s SATISFIABLE" << "blabla";
			break;
		case FALSE:
			cout << "s UNSATISFIABLE";
			break;
		case UNKNOWN:
			cout << "s ???";
	}
}

bool backtrack(Formula& f, stack<Decision>& decisions, ostream& os, Opt& option)
{
	do
	{
		const Decision dec = decisions.top();
		decisions.pop();
		cancel(dec.var,f,os,option.debug);

	}while(decisions.top().choice == INFER)

}

State Formula::test()
{
	this->resurrection();/** (accès simple ?) (peut-être cas particulier d'une partie de cancel ?)**/ //Remet toutes les clauses vivantes
	State sol = TRUE;
	for(auto c:clauses_alive)
	{
		State sol_c = FALSE;
		for(auto i:c)
		{
			if(assignment[i] == UNKNOWN && sol_c != TRUE)
				sol_c = UNKNOWN;
			if(assignment[i] == TRUE)
				sol_c = TRUE;
				//break
		}
		if(sol_c == UNKNOWN && sol != FALSE)
			sol = UNKNOWN;
		if(sol_c == FALSE)
			sol == FALSE;
			//break
	}
	return sol;
}
void pretreatment(Formula& f, ostream& os, Opt& option)
{
	/**Redondance / Renommage  = avant ?**/
	/**Pour le cnf, suffit de créer un vector**/
    f.supprTauto(os,option);
}

void Formule::supprTauto(ostream& os, Opt& option)
{
	struct new_clauses({});
	for(auto c:clauses)
	{
		map<int,bool> truc;/**réinitialisation ?**/ //true : x, false : x bar
		bool tauto = false;
		for(auto j = c.begin(),j!=c.end() && !tauto; j++)
		{
			if(truc.find(abs(*j)) && truc[abs(*j)] == (*j<0)) // l'opposé est présent
			{
				tauto = true;
			}
			else
			{
				truc[abs(*j)] = (*j>0);
			}
		}
		if(!tauto)
		{
			new_clauses.add(c);
		}
	}
	f.clear_c(new_clauses);
}

Res update(Formula& f, stack<Decision>& decisions, ostream& os, Opt& option)
{
	f.apply_modification(os,option);//On met à jour les clauses ici

	Res act = f.propagation_unitary(decisions,os,option); //On teste le résultat des modifications au passage
	if(act == ERROR || act == SUCCESS)
		return act;

	if(!option.lw)
	{
		Res act_aux = propagation_unique_polarity(decisions,os,option);
		if(act_aux != NOTHING)
			act = act_aux;
	}

	return act;
}

Res Formula::propagation_unitary(stack<Decision>& decisions, ostream& os, Opt& option)
{
	Res act = SUCCESS;

	for(auto c:clauses_alive)
	{
		switch(c.size())
		{
			case 0:
				return ERROR;
			case 1:
				act = NEW;
				int x = get(c); /**Depend de l'implémentation**/
				this->update_var(x);
				decisions.push((x,INFER));
				break;
			default:
				if(act != NEW)
					act = NOTHING;
		}
	}

	return act;
}

Res Formula::propagation_unique_polarity(stack<Decision>& decisions, ostream& os, Opt& option)
{
	vector<int> seen(size,0); //0 : Nothing spotted, 1 : x spotted, -1 : x bar spotted, 2 : both spotted

	for(auto c:clauses)
	{
		bool tauto = false;
		for(auto j = c.begin(),j!=c.end() && !tauto; j++)
		{
			if(seen[abs(*j)] == 2)
				continue;

			if(!seen[abs(*j)])
			{
				seen[abs(*j)] = j/(abs(*j)); // +/- 1
			}
			else if((seen[abs(*j)] < 0) != ((*j) < 0)) //Different signs
			{
				seen[abs(*j)] = 2;
			}
		}
	}

	Res act = NOTHING;

	for(int i = 0; i < size, i++)
	{
		if(abs(seen[i]) == 1)
		{
			int x = i*seen[i];
			this->update_var(x);
			decisions.push((x,INFER));
			act = NEW;
		}
	}

	return act;
}

int get_next_var(Formula& f, ostream& os, Opt& option)
{
	int x;
	switch(option.get)
	{
		default:
			x = f.get_fst_var();
	}
	return x;
}
