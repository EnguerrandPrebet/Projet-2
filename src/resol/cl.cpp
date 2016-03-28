#include "cl.hpp"

#include <iostream> //interface

#include "formula.hpp"
#include "option.hpp"

void interface(Option& option)
{
	char cmd;
	bool good_cmd = false;
	while(!good_cmd)
	{
		cout << "Welcome to the graphic interface of ClauseLearning.Inc" << endl;
		cout << endl << "Press g to see the conflict graph of this wrong answer" << endl;
		cout << "Press c to continue until the next conflitct" << endl;
		cout << "Press t to stop the interaction and go directly at the end" << endl;

		cin >> cmd;
		switch(cmd)
		{
			case 'g':
				show_graph();
				break;
			case 't':
				option.cl_interactive = false;
			//Pas besoin de break, les 2 ferment l'interface
			case 'c':
				good_cmd = true;
				break;
			default:
				good_cmd = false;
		}
	}
}

int clause_learning(Formula& f, ostream& os, Option& option)
{


	if(option.cl_interactive)
		interface(option); /**???Où dans le CL ? À la fin ?**/
	return 0;
}

void show_graph()
{
}

