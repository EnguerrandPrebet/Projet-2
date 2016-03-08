void dpll()
{
	pretreatment(f,os,option);

	stack<Decision> decisions({});
	bool sat_unk = true;
	truc action_result;

	var x = get_next_var(f,os,option);
	decisions.push((x,GUESS));
	do
	{
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
		else if(action_result == SUCCESS)
		{
			sat_unk = false;
		}


	}while(sat_unk)

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
