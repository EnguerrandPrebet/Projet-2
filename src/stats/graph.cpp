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

		double cases = 1;
		file_out << "cases	std	wl	rand	moms dlis" << endl;
		while(file_in >> minutes >> seconds)
		{
			for(double i = -2; i < 2; i++)
			{
				seconds += 60*minutes;	
				
				file_out << cases+ i/8 << " ";
				file_out << seconds << " ";
				file_in >> minutes >> seconds;
			}
			seconds += 60*minutes;
			file_out << cases+ 2./8. << " ";
			file_out << seconds << endl;
			cases++;
		}

		file_in.close();
		file_out.close();
	}
}
