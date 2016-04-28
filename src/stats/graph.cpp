#include <iostream>
#include <fstream>

using namespace std;

int main(int argc,char* argv[])
{
	ifstream file_in("stats.dat",ios::in);
	ofstream file_out("graph.dat",ios::out);
	if(file_in && file_out)
	{
		int minutes;
		double seconds;

		int cases = 1;
		file_out << "cases	std	wl	rand	moms dlis" << endl;
		while(file_in >> minutes >> seconds)
		{
			file_out << cases << " ";
			for(int i = 0; i < 4; i++)
			{
				seconds += 60*minutes;
				file_out << seconds << " ";
				file_in >> minutes >> seconds;
			}
			seconds += 60*minutes;
			file_out << seconds << endl;
			cases++;
		}

		file_in.close();
		file_out.close();
	}
}
