#include "cl.hpp"

#include <iostream> //interface
#include <queue> //bfs

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
	vector<list<int>> la; //graphe des sommets bleus
	vector<list<int>> la_inv;
	vector<list<int>> la_old; //graphe qui à un sommet bleu donne les parents blancs

	create_graphe(la,la_inv,la_old,f);

	if(option.cl_interactive)
		interface(option); /**???Où dans le CL ? À la fin ?**/
	return 0;
}

void create_graphe(vector<list<int>>& la, vector<list<int>>& la_inv, vector<list<int>>& la_old, Formula& f)
{

}

void show_graph()
{
}

enum Color{BLUE,YELLOW,PURPLE};

void dfs(int i, vector<list<int>>& la, vector<pair<int,int>>& time, int& t);
int bfs(int root, vector<list<int>>& la_inv, vector<pair<int,int>>& time);
bool isuip(int challenger, vector<pair<int,int>>& time);
void apply_color(int i, vector<list<int>>& la, Color new_color, vector<Color>& color);

vector<Color> coloring(vector<list<int>> la, vector<list<int>> la_inv, int root)
{
	vector<Color> color(la.size(),BLUE);
	vector<pair<int,int>> time(la.size(),make_pair(-1,-1));
	int t = 0;
	dfs(root,la,time,t);

	int uip = bfs(0,la_inv,time);

	apply_color(uip,la,PURPLE,color);
}

void dfs(int i, vector<list<int>>& la, vector<pair<int,int>>& time, int& t)
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

int bfs(int root, vector<list<int>>& la_inv, vector<pair<int,int>>& time)
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


}

bool isuip(int challenger, vector<pair<int,int>>& time)
{
	for(int i = 0; i < time.size(); i++)
	{
		if((time[i].first < time[challenger].first || time[i].second > time[challenger].second)&&(time[i].first > time[challenger].first || time[i].second < time[challenger].second))
			return false;
	}
	return true;
}

void apply_color(int i, vector<list<int>>& la, Color new_color, vector<Color>& color)
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
