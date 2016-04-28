#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

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
			string file_name = path + to_string(z) + ".cnf";

			ofstream file;
			file.open(file_name);

			file << "p cnf " << c << " " << v << endl;
			for(int i = 0; i < c; i++)
			{
				int taille = (rand()%size_c)+1;
				for(int j = 0; j < taille; j++)
				{
					int var = (rand()%v)+1;
					int lit = var*(2*(rand()%2) - 1);
					file << lit << " ";
				}
				file << "0" << endl;
			}

			file.close();
        }
	}

}
