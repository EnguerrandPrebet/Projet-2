enum Res {NEW,NOTHING,ERROR,SUCCESS};

void dpll()
{

	pretreatment(f,os,option);

	stack<Decision> decisions({});

	bool sat_unk = true;
	while(sat_unk)
	{
		Res action_result; //On commence dans un état stable (-> pari)

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

bool backtrack(f,decisions,os,option)
{
	do
	{
		const Decision dec = decisions.top();
		decisions.pop();
		cancel(dec.var,f,os,option.debug);

	}while(decisions.top().choice == INFER)

}

State Formule::test()
{
	State sol = TRUE;
	for(auto c:clauses)
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
void pretreatment(f,os,option)
{
	/**Redondance / Renommage  = avant ?**/
    f.supprTauto(os,option);
}

void Formule::supprTauto(os,option)
{
	map<int,bool> truc; //true : x, false : x bar
	struct new_clauses({});
	for(auto c:clauses)
	{
		bool tauto = false;
		for(auto j = c.begin(),j!=c.end() && !tauto; j++)
		{
			if(truc[j] == (j<0)) // l'opposé est présent
			{
				tauto = true;
			}
			else
			{
				truc[j] = (j>0);
			}
		}
		if(!tauto)
		{
			new_clauses.add(c);
		}
	}
	f.new_c(new_clauses);
}
