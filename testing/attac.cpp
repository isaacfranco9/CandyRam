#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

float getAngle(float, float);

int main()
{
	ifstream inData;
	system("./repacc.sh");
	inData.open("acc.log");
	
	if (inData.is_open())
	{
		cout << "giggity " << endl;
		string Xraw, Zraw, Yraw;
		inData >> Xraw;
		inData >> Yraw;
		inData >> Zraw;
		
		float x = stof(Xraw);
		float y = stof(Yraw);
		float z = stof(Zraw);
		
		cout << Xraw << " " << Yraw  << " " << Zraw << endl;
		// Old		Roll					  Pitch						Yaw
		cout << getAngle(-x, z) << " " << getAngle(y, z) << " " << getAngle(x, y) << endl;
		// New		Roll					  Pitch						Yaw
		cout << getAngle(z, y) << " " << getAngle(-x, y) << " " << getAngle(-z, -x) << endl;
	}
	
	inData.close();
	return 0;
}

// Find the angle around a unit circle from (0, 1) to (a, b) normalized.
float getAngle(float a, float b)
{
	// Normalize the values
	float magnitude = sqrt(a * a + b * b);
	if (magnitude != 0)
	{
		a /= magnitude;
		b /= magnitude;
	}
	else
	{
		return 0;
	}
	
	// Find the sign of the angle
	int sign = (a >= 0) - (a < 0);
	
	// Find the angle
	return sign * acos(1 - (a * a + (1 - b) * (1 - b)) / 2) * 57.296;
}
