#include <iostream>
#include <wiringPi.h>

const int PULSE_WIDTH_MIN = 925;
const int PULSE_WIDTH_MAX = 2000;

int main()
{
	wiringPiSetup(); // Call when setting pins to input or out
	
	// Set physical pin 22 to output mode BCM 25
	//GPIO 5 (PHYSICAL 18) IS FUCKED
	int pinNum = 6;
	pinMode(pinNum, OUTPUT);
	
	// Delay
	//delay(180000);
	
	// Retract
	int retractionTime = 10;
	int retractionCycleLength = PULSE_WIDTH_MIN * 5;
	int retractionPulses = retractionTime * 1000000 / retractionCycleLength;
	for (int p = 0; p < retractionPulses; p++)
	{
		digitalWrite(pinNum, HIGH);
		
		delayMicroseconds(PULSE_WIDTH_MIN);
	
		digitalWrite(pinNum, LOW);

		delayMicroseconds(PULSE_WIDTH_MIN * 4);
	}
	/*
	// Extend in intervals
	int totalExtensiomTime = 40;
	int intervals = 4;
	for (int i = 1; i <= intervals; i++)
	{
		int pulseWidth = PULSE_WIDTH_MIN + i * ((PULSE_WIDTH_MAX - PULSE_WIDTH_MIN) / intervals);
		int cycleLength = pulseWidth * 2;
		int intervalPulses = totalExtensiomTime * 1000000 / intervals / cycleLength;
		
		for (int p = 0; p < intervalPulses; p++)
		{
			digitalWrite(pinNum, HIGH);
		
			delayMicroseconds(pulseWidth);
	
			digitalWrite(pinNum, LOW);

			delayMicroseconds(pulseWidth);
		}
	}*/

	return 0;
}
