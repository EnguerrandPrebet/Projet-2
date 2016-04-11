#include "cl.hpp"

#include <iostream> // interface
#include <queue> // bfs
#include <fstream> // pour créer le fichier .dot

#include <cstdlib> // pour lancer graphviz sur le fichier .dot

#include "formula.hpp"
#include "option.hpp"


enum Color {NO_COLOR, WHITE, BLUE, YELLOW, PURPLE, NEW_CLAUSE}; //!Le dernier sert dans la création de la nouvelle clause pour les redondances, doit être considéré comme identique à WHITE

void show_graph(const vector< list<int> >& la, const vector<Color>& color);

void interface(const vector< list<int> >& la, const vector<Color>& color, Option& option)
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
				show_graph(la, color);
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

int create_graphe(vector< list<int> >& la, vector< list<int> >& la_inv, vector< list<int> >& la_old, vector<Color>& color_v, stack<Decision_var> decisions);
int get_uip(const vector< list<int> >& la, const vector< list<int> >& la_inv, const int& root);
void merge(vector< list<int> >& la, const vector< list<int> >& la_old);
void apply_color(const int& i, const vector< list<int> >& la, const Color& new_color, vector<Color>& color);

int generate_new_clause(Formula& f, Clause& clause_learned, const vector< list<int> >& la_old, vector<Color>& color_v, int& uip);

int clause_learning(Formula& f, const stack<Decision_var>& decisions, Clause& clause_learned, ostream& os, Option& option)
{
	//0 représente le conflit
	vector< list<int> > la(f.nb_variables()+1); //graphe des sommets bleus
	vector< list<int> > la_inv(f.nb_variables()+1);
	vector< list<int> > la_old(f.nb_variables()+1); //graphe qui à un sommet bleu donne les parents blancs (donc inversé)

	vector<Color> color_v(f.nb_variables()+1,NO_COLOR);
	int root = create_graphe(la, la_inv, la_old, color_v, decisions);/*renvoie l'origine du graphe bleus, aka le pari.*///!En coloriant au passage en blanc et bleu

	int uip = get_uip(la, la_inv, root);

	apply_color(uip,la,PURPLE,color_v);
	color_v[uip] = YELLOW;

	int time_back;

	time_back = generate_new_clause(f, clause_learned, la_old, color_v, uip); ///Ajoute la clause à f

	merge(la,la_old); //Rajoute les aretes de la_old à l'endroit et les ajoute dans la
	if(option.cl_interactive)
		interface(la, color_v, option); /**???Où dans le CL ? À la fin ?**/

	return time_back;
}

vector<bool> update_cancel(const int& n, stack<Decision_var> decisions);

int create_graphe(vector< list<int> >& la, vector< list<int> >& la_inv, vector< list<int> >& la_old, vector<Color>& color_v, stack<Decision_var> decisions) /**Pas de copie du stack pour pas niquer le backtrack**/
{
	int current_time = decisions.top().time;

	vector<bool> be_cancelled = update_cancel(la.size(), decisions);//Pour détecter les sommets bleus

	int x_fils = 0; //On le définit maintenant car à la fin de la boucle, x_fils == pari //initialisation anti-warning

	while(!decisions.empty() && decisions.top().time >= current_time)
	{
		Decision_var dec = decisions.top();
		decisions.pop();

		Clause c = dec.reason;
		x_fils = abs(dec.var);
		stack<int> stack_delete = c.get_stack();

		while(!stack_delete.empty() && be_cancelled[abs(stack_delete.top())])
		{
			int x_pere = abs(stack_delete.top());
			stack_delete.pop();

			la[x_pere].push_back(x_fils);
			la_inv[x_fils].push_back(x_pere);

			color_v[x_pere] = WHITE;
			color_v[x_fils] = WHITE;
		}
		//Et maintenant les sommets blancs
		while(!stack_delete.empty())
		{
			int x_pere = abs(stack_delete.top());
			stack_delete.pop();

			la_old[x_fils].push_back(x_pere);
			color_v[x_pere] = BLUE;
		}
	}

	return x_fils;
}

vector<bool> update_cancel(const int& n, stack<Decision_var> decisions) //Toujours une copie
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

void show_graph(const vector< list<int> >& la, const vector<Color>& color)
{
	unsigned int n = color.size();

	ofstream file("graph.dot", ifstream::out | ifstream::trunc);

	if (!file.good())
		cout << "Error: can't open graph.dot for reading" << endl;

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
			case PURPLE: graphviz_color_name = "thistle"; break;

			case NEW_CLAUSE: graphviz_color_name = "crimson"; break;
			default: continue; // variable non concernée
		}

		file << "\t" << s << " [style=filled, fillcolor=" << graphviz_color_name << "];" << endl;
	}

	file << endl;

	/* On ajoute les arrêtes */
	for (unsigned int s1 = 0; s1 < n; s1++)
	{
		for (unsigned int s2 : la[s1])
			file << "\t" << s1 << " -> " << s2 << ";" << endl;
	}

	file << "}";

	file.close();

	/* On affiche le graphe */
	system("dot -Tps graph.dot -o graph.ps");
	remove("graph.dot");
	system("xdg-open graph.ps");
	remove("graph.ps");
}

void dfs(const int& i, const vector< list<int> >& la, vector<pair<int,int>>& time, int& t);
int bfs(const int& root, const vector< list<int> >& la_inv, const vector<pair<int,int>>& time);
bool isuip(const int& challenger, const vector<pair<int,int>>& time);

int get_uip(const vector< list<int> >& la, const vector< list<int> >& la_inv, const int& root)
{
	vector<pair<int,int>> time(la.size(),make_pair(-1,-1));
	int t = 0;
	dfs(root,la,time,t);

	int uip = bfs(0,la_inv,time);

	return uip;
}

void dfs(const int& i, const vector< list<int> >& la, vector<pair<int,int>>& time, int& t)
{
    time[i].first = t;
    t++;
    for(int j:la[i])
    {
		if(time[j].first == -1)
		{
			dfs(j,la,time,t);
		}
    }
    time[i].second;
}

int bfs(const int& root, const vector< list<int> >& la_inv, const vector<pair<int,int>>& time)
{
	vector<bool> isSeen(time.size(),false);
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
		if(isuip(challenger,time))
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

bool isuip(const int& challenger, const vector<pair<int,int>>& time)
{
	for(unsigned int i = 0; i < time.size(); i++)
	{
		//On vérifie que i est un ancêtre du conflit (pas une déduction qui ne mènent nul part, et que challenger est un parent (ancêtre ou descendant) de i
		if((time[i].first < time[0].first && time[i].second > time[0].second) && ((time[i].first < time[challenger].first || time[i].second > time[challenger].second) && (time[i].first > time[challenger].first || time[i].second < time[challenger].second)))
			return false;
	}
	return true;
}

void apply_color(const int& i, const vector< list<int> >& la, const Color& new_color, vector<Color>& color)
{
	color[i] = new_color;
    for(int j:la[i])
    {
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

int generate_new_clause(Formula& f, Clause& clause_learned, const vector< list<int> >& la_old, vector<Color>& color_v, int& uip)
{
	list<int> clause;

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

	return f.generate_new_clause(clause,uip, clause_learned);
}
