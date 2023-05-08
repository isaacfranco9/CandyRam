#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
using namespace std;

int lerp(int, int, float);

int main()
{
	wiringPiSetup(); // Call when setting pins to input or out
	
	int speedPin = 2; // 27
	int in1Pin = 7; // 4
	int in2Pin = 22; // 6
	
	// Set pins to output
	pinMode(speedPin, OUTPUT);
	pinMode(in1Pin, OUTPUT);
	pinMode(in2Pin, OUTPUT);
	
	// 1 low 2 high = reverse, 1 high 2 low = forward, same = off
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, HIGH);
	
	// PWM for speed
	// Duty cycle controls speed as percent (pulse width / cycle length)
	/*for (int i = 0; i < 10000; i++)
	{
		digitalWrite(speedPin, HIGH);

		delayMicroseconds(100);
	
		digitalWrite(speedPin, LOW);

		delayMicroseconds(180);
	}*/
	
	// Full speed
	digitalWrite(speedPin, HIGH);
	delay(28000);
	digitalWrite(speedPin, LOW);
	
	// Turn input pins off
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, LOW);
	
	return 0;
}
