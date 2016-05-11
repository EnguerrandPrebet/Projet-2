#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

string usage = "Usage: generator nombre_de_clause nombre_de_variable nombre_max_de_variable_par_clause (chemin = .) (nombre_de_tests = 1)";

int main(int argc, char* argv[])
{
	if (argc < 4 || argc > 6)
		cout << usage << endl;
	else
	{
		int c = atoi(argv[1]), v = atoi(argv[2]), size_c = atoi(argv[3]), num_test = 1;
		string path = "./";
		if(argc >= 5)
			path = argv[4];
		if(argc == 6)
			num_test = atoi(argv[5]);

		srand(time(NULL));

        for(int z = 0; z < num_test; z++)
        {
			int mini = size_c + 1;
			string file_name = path + to_string(z) + ".cnf";

			ofstream file;
			file.open(file_name);

			file << "p cnf " << v << " " << c << endl;
			for(int i = 0; i < c; i++)
			{
				int taille = (rand()%(size_c/2))+ size_c/2;

				mini = min(taille,mini);

				vector<int> affected(v,0);

				for(int j = 0; j < taille; j++)
				{
					int var = (rand()%v)+1;
					if(affected[var])
						file << var*affected[var] << " ";
					else
					{
						int neg = 2*(rand()%2) - 1;
						affected[var] = neg;
						file << neg*var << " ";
					}

				}
				file << "0" << endl;
			}

			cout << mini << endl;
			file.close();
        }
	}

}
