#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
using namespace std;

int lerp(int, int, float);

// Most consistent setup for continuous servo is
// 28 cycles on 3ms cycle
// 1400 = Counter CW, 1600 = CW
// for 60 +- 5 degree rotation.

int main()
{
	wiringPiSetup(); // Call when setting pins to input or out
	
	// Set GPIO pin to PWM mode
	pinMode(1, OUTPUT);
	
	int iterations = 50; //28
	int angle = 180;
	
	for (int i = 0; i < iterations; i++)
	{
		digitalWrite(1, HIGH);
		
		// Range: (0) 500-2500 (180)
		delayMicroseconds(500 + angle * (2000 / 180));
	
		digitalWrite(1, LOW);
		
		// Range: (180) 17500-19500 (0)
		delayMicroseconds(19500 - angle * (2000 / 180));
	}
	
	return 0;
}

int lerp (int start, int end, float t)
{
	return start + (end - start) * t;
}
