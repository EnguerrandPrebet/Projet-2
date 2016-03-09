void dpll()
{

	bool sat_unk = (pretreatment(f,os,option) == UNKNOWN);

	stack<Decision> decisions({});

	truc action_result = NOTHING; //On commence dans un état stable (-> pari)

	while(sat_unk)
	{

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

		do // NEW : nouvelle déduction, NOTHING : rien, ERROR : Non satisfiable, SUCCESS : On a gagné !
		{
			action_result = update(f,decisions,os,option);
		}while(action_result==NEW)

	}

	//Peut être hors de la fonction DPLL (même si je ne vois pas pourquoi)
	switch(test(f))
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
