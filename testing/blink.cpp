#include <iostream>
#include <wiringPi.h>
using namespace std;


int main()
{
	// Array includes all GPIO pins
	int testPinWpiNums[] = { 0, 1, 2, 3, 4, 5, 6, 7, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
	
	wiringPiSetup(); // Call when setting pins to input or out
	
	// Set pins to output mode, call with pin number according to wPi, goes to BCM pin on board (gpio readall)
	for (int pinNum : testPinWpiNums)
	{
		pinMode(pinNum, OUTPUT);
	}
	
	// Set pin voltages to high
	for (int pinNum : testPinWpiNums)
	{
		digitalWrite(pinNum, HIGH);
	}
	
	// Delay one second
	delay(1000);
	
	// Set pin voltages to low
	for (int pinNum : testPinWpiNums)
	{
		digitalWrite(pinNum, LOW);
	}
}
