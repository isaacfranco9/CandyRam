#include <iostream>
#include <fstream>
#include <string>
#include <wiringPi.h>
#include <string> //for good measure
#include <cmath>
#include <algorithm>
#include <ctime>
using namespace std;

string getTimeStamp()
{
	time_t rawtime = time(0);
	struct tm * timeInfo = gmtime(&rawtime);
	char buffer[22];
	strftime(buffer, 22, "[%Y-%m-%d_%T]", timeInfo);
	return buffer;
}

int main()
{
	
	string timestamp = getTimeStamp().substr(1,10);
	//timestamp+= ".log";
	cout << timestamp << endl;
	string str;
		//iteration++;
	//string name = "Name.txt";
	ifstream nombre;
	//system("ls -d 2023* > Name.txt");
	
	nombre.open(timestamp + ".log");	
	getline(nombre, str);
	
	cout << str << endl;
		
	nombre.close();
		

}
