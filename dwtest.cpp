//created to start direwolf check the file every second until a transmission with our callsign is found.
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <wiringPi.h>
using namespace std;

int main()
{
	wiringPiSetup();
	system("./dw-start.sh");
	// Time in milliseconds
	const int TRANSMISSION_CHECK_DELAY = 1000;
	
	// The callsign from which a transmission will be accepted
	const string CALLSIGN = "RR4RAM";
	
	string transmission = "";
	string recievedCallsign = "";
	
	while (recievedCallsign != CALLSIGN)
	{
		delay(TRANSMISSION_CHECK_DELAY);
		
		// Isaac code
		string str;
		char stop;
		bool no = true;
		ifstream direlog;
		direlog.open("2023-03-16.log");
		while(!direlog.eof())
		{	
			transmission = "";
			int comm=0;
			getline(direlog, str);
			for(int i = 0; i< str.length(); i++)
			{
				if(comm<=20)
				{
					if(str[i] == ',')
					{
						comm ++;
					}
				}
				else // Is comment
				{
					transmission += str[i];
				}	
			}
			
			// Remove space at beginning if it exists
			if (transmission[0] == ' ')
				transmission = transmission.substr(1, transmission.length() - 1);
			
			// Check for correct callsign
			if (transmission.length() >= 6)
			{
				recievedCallsign = transmission.substr(0, 6);
				if (recievedCallsign == CALLSIGN)
					break;
			}
		}
		
		direlog.close();
		// No mo Isaac code
	}
	
	cout << transmission << endl;
	
	return 0;
}

