#include "cl.hpp"

#include "formula.hpp"
#include "global.hpp"

#include <iostream> // interface
#include <queue> // bfs
#include <fstream> // pour créer le fichier .dot

#include <cstdlib> // pour lancer graphviz sur le fichier .dot

enum Color {NO_COLOR, PRE_WHITE, PRE_BLUE, WHITE, BLUE, YELLOW, PURPLE, NEW_CLAUSE}; //NEW_CLAUSE = GREEN
/*PRE_WHITE et PRE_BLUE sont les couleurs du graphe complet (de l'état courant)*/
/*Seuls les sommets liés au conflit passent à la couleur WHITE,BLUE*/

void show_graph(const Formula& f, const vector< list<int> >& la, const vector<Color>& color);

void interface(const Formula& f, const vector< list<int> >& la, const vector<Color>& color, int time_back)
{
	char cmd;
	bool good_cmd = false;

	cout << "Welcome to the graphic interface of ClauseLearning.Inc" << endl;
	cout << endl << "Press g to see the conflict graph of this wrong answer" << endl;
	cout << "Press c to continue until the next conflitct" << endl;
	cout << "Press t to stop the interaction and go directly at the end" << endl;
	cout << endl << "We go back to " << time_back << "." << endl;

	while(!good_cmd)
	{
		cin >> cmd;
		switch(cmd)
		{
			case 'g':
				show_graph(f, la, color);
				break;

			case 't':
				Global::option.cl_interactive = false;
			//Pas besoin de break, les 2 ferment l'interface
			case 'c':
				good_cmd = true;
				break;

			default:
				good_cmd = false;
		}
	}
}

int create_graphe(vector< list<int> >& la_inv, vector<Color>& color_v, stack<Decision_var> decisions);
void refine_graphe(int i, vector< list<int> >& la, vector< list<int> >& la_inv, vector< list<int> >& la_old, vector<Color>& color_v);
int get_uip(const vector< list<int> >& la, const vector< list<int> >& la_inv, int root);
void merge(vector< list<int> >& la, const vector< list<int> >& la_old);
void apply_color(int i, const vector< list<int> >& la, const Color& new_color, vector<Color>& color);

int generate_new_clause(Formula& f, Clause& clause_learned, const vector< list<int> >& la_old, vector<Color>& color_v, int uip);

int clause_learning(Formula& f, const stack<Decision_var>& decisions, Clause& clause_learned)
{
	//0 représente le conflit
	vector< list<int> > la(f.nb_variables()+1); //graphe des sommets bleus
	vector< list<int> > la_inv(f.nb_variables()+1);
	vector< list<int> > la_old(f.nb_variables()+1); //graphe qui à un sommet bleu donne les parents blancs (donc inversé)

	vector<Color> color_v(f.nb_variables()+1, NO_COLOR);
	int root = create_graphe(la_inv, color_v, decisions);/*renvoie l'origine du graphe bleus, aka le pari.*///En coloriant au passage en blanc et bleu
	refine_graphe(0, la, la_inv, la_old, color_v);

	int uip = get_uip(la, la_inv, root);

	apply_color(uip,la,PURPLE,color_v);
	color_v[uip] = YELLOW;

	int time_back;

	time_back = generate_new_clause(f, clause_learned, la_old, color_v, uip); ///Ajoute la clause à f

	merge(la, la_old); //Rajoute les aretes de la_old à l'endroit et les ajoute dans la
	if(Global::option.cl_interactive)
		interface(f, la, color_v, time_back);

	return time_back;
}

vector<bool> update_cancel(int n, stack<Decision_var> decisions);

int create_graphe(vector< list<int> >& la_inv, vector<Color>& color_v, stack<Decision_var> decisions) /**Pas de copie du stack pour pas niquer le backtrack**/
{
	int current_time = decisions.top().time;
	vector<bool> be_cancelled = update_cancel(la_inv.size(), decisions);//Pour détecter les sommets bleus

	int x_fils = 0; //On le définit maintenant car à la fin de la boucle, x_fils == pari //initialisation anti-warning

	Global::DEBUG(1) << "graph creation" << current_time << endl;
	while(!decisions.empty() && decisions.top().time >= current_time)
	{
		Decision_var dec = decisions.top();
		decisions.pop();

		Clause c = dec.reason;
		x_fils = abs(dec.var);
		Global::DEBUG(3) << "loop " << x_fils << endl;

		vector<int> clause = c.get_lit_fixed();
		for(int i:clause)
		{
			Global::DEBUG(3) << i << " ";
		}
		Global::DEBUG(3) << endl;
		for(int i:clause)
		{
			int x_pere = abs(i);
			if(x_fils == x_pere)
				continue;
			if(be_cancelled[x_pere])
			{
				la_inv[x_fils].push_back(x_pere);
				Global::DEBUG(3) << x_fils << " " << x_pere << endl;
				color_v[x_pere] = PRE_BLUE;
				color_v[x_fils] = PRE_BLUE;
			}
			else
			{
				la_inv[x_fils].push_back(x_pere);
				color_v[x_pere] = PRE_WHITE;
			}

		}
		/*
		stack<int> stack_delete = c.get_stack();

		while(!stack_delete.empty() && be_cancelled[abs(stack_delete.top())])
		{
			int x_pere = abs(stack_delete.top());

			stack_delete.pop();

			la_inv[x_fils].push_back(x_pere);

			color_v[x_pere] = PRE_BLUE;
			color_v[x_fils] = PRE_BLUE;
		}

		//Et maintenant les sommets blancs
		while(!stack_delete.empty())
		{
			int x_pere = abs(stack_delete.top());

			stack_delete.pop();

			la_inv[x_fils].push_back(x_pere);
			color_v[x_pere] = PRE_WHITE;
		}
		*/
	}

	return x_fils;
}

vector<bool> update_cancel(int n, stack<Decision_var> decisions) //Toujours une copie
{
	int current_time = decisions.top().time;

	vector<bool> be_cancelled(n, false);

	while(!decisions.empty() && decisions.top().time >= current_time)
	{
		Decision_var dec = decisions.top();
		decisions.pop();

		be_cancelled[abs(dec.var)] = true;
	}

	return be_cancelled;
}

void refine_graphe(int i, vector< list<int> >& la, vector< list<int> >& la_inv, vector< list<int> >& la_old, vector<Color>& color_v)
{
	color_v[i] = BLUE;

	Global::DEBUG(3) << " affinage " << i << " ";

	for(int j:la_inv[i])
	{
		Global::DEBUG(3) << "(" << i << "," << j << ") ";

		if(color_v[j] == PRE_WHITE || color_v[j] == WHITE)
		{
			color_v[j] = WHITE;
			la_old[i].push_back(j);
		}
		else
		{
			la[j].push_back(i);
			if(color_v[j] == PRE_BLUE)
				refine_graphe(j,la,la_inv,la_old,color_v);
		}
	}

	Global::DEBUG(3) << endl;
}

void show_graph(const Formula& f, const vector< list<int> >& la, const vector<Color>& color)
{
	unsigned int n = color.size();

	ofstream file("graph.dot", ifstream::out | ifstream::trunc);

	if (!file.good())
	{
		Global::ERROR() << "can't open graph.dot for reading" << endl;
		exit(1);
	}

	file << "digraph \"Conflict Graph\" {" << endl;

	/* On set les couleurs des noeuds */
	for (unsigned int s = 0; s < n; s++)
	{
		string graphviz_color_name;
		switch (color[s])
		{
			case WHITE: graphviz_color_name = "white"; break;
			case BLUE: graphviz_color_name = "lightblue"; break;
			case YELLOW: graphviz_color_name = "khaki"; break;
			case PURPLE: graphviz_color_name = "lightblue"; break; //avant : "thistle"

			case NEW_CLAUSE: graphviz_color_name = "palegreen3"; break;
			default: continue; // variable non concernée
		}

		string conflict_style;
		if (s == 0)
		{
			conflict_style = "label=\"CONFLICT\",";
			graphviz_color_name = "tomato1";
		}

		string neg_string = (s != 0 && f.variable_assignment(s) == FALSE) ? "¬" : "";

		string t = (f.renaming.is_input_variable(s)) ? "" : "t";
		Real_Value vertex_label = (f.renaming.is_input_variable(s)) ? f.renaming.inverse_translate_litteral(s) : (s - f.renaming.number_of_input_variables());

		string conflict_related_node_style = (color[s] == PURPLE || color[s] == YELLOW || color[s] == NEW_CLAUSE) ? ",color=tomato1,penwidth=2" : "";

		file << "\t" << s << " [label=\"" << neg_string << t << vertex_label << "\"," << conflict_style << "style=filled, fillcolor=" << graphviz_color_name << conflict_related_node_style << "];" << endl;
	}

	file << endl;

	/* On ajoute les arrêtes */
	for (unsigned int s1 = 0; s1 < n; s1++)
	{
		for (unsigned int s2 : la[s1])
		{
			bool conflict_related_edge =
					(color[s1] == PURPLE || color[s1] == YELLOW || color[s1] == NEW_CLAUSE)
					&& (color[s2] == PURPLE || color[s2] == YELLOW || color[s2] == NEW_CLAUSE);

			string conflict_edge_style = (conflict_related_edge) ? "[color=tomato1]" : "";

			file << "\t" << s1 << " -> " << s2 << conflict_edge_style << ";" << endl;
		}
	}

	file << "}";

	file.close();

	/* On affiche le graphe */
	system("dot -Tps graph.dot -o graph.ps");
	//remove("graph.dot");
	system("xdg-open graph.ps");
	//remove("graph.ps");
}

void dfs(int i, const vector< list<int> >& la, vector< vector<bool> >& dependance, vector< vector<bool> >& obligation);
int bfs(int root, const vector< list<int> >& la_inv, const vector< vector<bool> >& dependance, const vector< vector<bool> >& obligation);
bool isuip(int challenger, const vector< vector<bool> >& dependance, const vector< vector<bool> >& obligation);

int get_uip(const vector< list<int> >& la, const vector< list<int> >& la_inv, int root)
{
	vector< vector<bool> > dependance(la.size(),vector<bool>(la.size(),false)), obligation(la.size(),vector<bool>(la.size(),false));
	dfs(root,la,dependance,obligation);


	int uip = bfs(0,la_inv,dependance,obligation);

	return uip;
}

void dfs(int i, const vector< list<int> >& la, vector< vector<bool> >& dependance, vector< vector<bool> >& obligation)
{
	dependance[i][i] = true;

	obligation[i][i] = true;

	Global::DEBUG(3) << " uip " << i << " ";

	for(int j:la[i])
	{
		Global::DEBUG(3) << "(" << i << "," << j << ") ";
		if(dependance[j][j] == false)
		{
			dfs(j,la,dependance,obligation);
		}
		for(unsigned int k = 0; k < dependance.size(); k++)
		{
			if(dependance[j][k])
				dependance[i][k] = true;
		}
		Global::DEBUG(3) << endl;
	}
	for(unsigned int k = 0; k < dependance.size(); k++)
	{
		if(!dependance[i][k])
			continue;

		bool obl = true;
		for(int j: la[i])
		{
			if(obligation[j][k] == false && dependance[j][0])
				obl = false;
		}
		if(obl)
			obligation[i][k] = true;
	}

}

int bfs(int root, const vector< list<int> >& la_inv, const vector< vector<bool> >& dependance, const vector< vector<bool> >& obligation)
{
	vector<bool> isSeen(dependance.size(),false);
	isSeen[root] = true;
	queue<int> q;
	for(int i:la_inv[root])
	{
		q.push(i);
		isSeen[i] = true;
	}

	while(!(q.empty()))
	{
		int challenger = q.front();
		isSeen[challenger] = true;
		q.pop();
		if(isuip(challenger,dependance,obligation))
			return challenger;

		for(int i:la_inv[challenger])
		{
			if(isSeen[i] == false)
			{
				q.push(i);
				isSeen[i] = true;
			}
		}
	}

	return 0;
}

bool isuip(int challenger, const vector< vector<bool> >& dependance, const vector< vector<bool> >& obligation)
{
	for(unsigned int i = 0; i < dependance.size(); i++)
	{
		//On vérifie que i est un ancêtre du conflit (pas une déduction qui ne mènent nul part, et que challenger est soit un ancêtre de i, soit i est obligé de passer par challenger pour aller à 0
		if(dependance[i][0] && !((dependance[i][challenger] && obligation[i][challenger]) || dependance[challenger][i]))
			return false;
	}
	return true;
}

void apply_color(int i, const vector< list<int> >& la, const Color& new_color, vector<Color>& color)
{
	Global::DEBUG(3) << " color " << i << " ";
	color[i] = new_color;
	for(int j:la[i])
	{
		Global::DEBUG(3) << "(" << i << "," << j << ") ";
		if(color[j] != new_color)
		{
			apply_color(j,la,new_color,color);
		}
	}
}

void merge(vector< list<int> >& la, const vector< list<int> >& la_old)
{
	for(unsigned int i = 0; i < la_old.size(); i++)
	{
		for(int j: la_old[i])
		{
			la[j].push_back(i);
		}
	}
}

int generate_new_clause(Formula& f, Clause& clause_learned, const vector< list<int> >& la_old, vector<Color>& color_v, int uip)
{
	vector<int> clause; //On a besoin de trier les éléments dans la méthode d'où un vector (pour utiliser sort de algorithm)

	for(unsigned int i = 0; i < color_v.size(); i++)
	{
		if(color_v[i] == PURPLE)
		{
			for(int j: la_old[i])
			{
				if(color_v[j] == WHITE)//Sommet pas encore vu
				{
					clause.push_back(j);
					color_v[j] = NEW_CLAUSE;
				}
			}
		}
	}

	return f.add_learned_clause(clause,uip, clause_learned);
}
