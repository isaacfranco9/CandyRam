#include <iostream>
#include <fstream>
#include <string>
#include <wiringPi.h>
#include <string> //for good measure
#include <cmath>
#include <algorithm>
#include <ctime>
using namespace std;

/* Isaac Franco & Braxton Fox
An example transmission sequence could look something like, "XX4XXX C3 A1 D4 C3 F6 C3 F6 B2 B2 C3".
Note the call sign that NASA will use shall be distributed to teams at a later time.
*/

// Utility function prototypes
string getTimestamp();
string getDirewolfDate();
string getDate();
float clamp(float, float, float);
float getAngle(float, float);
float readAccelerometerAxis(char);
void rotateServo60(int, bool);
void setServoAngle(int, float);

// Main step function prototypes
int pinBcmToWpi(int);
void extendLinearActuator(int);
void raisePayload(int, int, int);
void lowerPayload(int, int, int);
void orientCamera(int, int);
string retrieveTransmission();
void executeCommand(string, int, float&);

// Delay before the deployment of the payload after the Raspberry Pi is turned on
const int DEPLOYMENT_DELAY = 0;

// Values are in degrees
const int ROLL_MIN = 60;
const int ROLL_MAX = 180;
const int ROLL_DEFAULT = 150;
const int ROLL_DEPLOY = 180;
const int PITCH_MIN = 30;
const int PITCH_MAX = 90;
const int PITCH_DEFAULT = 60;
const int PITCH_DEPLOY = 40;

// Pin numbers as they appear on the Raspberry Pi
const int LIN_ACT_PIN_BCM = 25; //was 24 but 24 (physical 18) doesnt work consistently
const int DC_MOTOR_SPD_PIN_BCM = 27;
const int DC_MOTOR_IN1_PIN_BCM = 6;
const int DC_MOTOR_IN2_PIN_BCM = 4;
const int ROLL_PIN_BCM = 18;
const int PITCH_PIN_BCM = 14;
const int YAW_PIN_BCM = 15;

// All times are in milliseconds unless otherwise specified
// Pitch axis is x, roll axis is y, yaw axis is z
int main()
{
	cout << getTimestamp() << " Payload handler active.\n";
	
	cout << getTimestamp() << " Delaying " << DEPLOYMENT_DELAY / 1000.0 << " seconds...\n";
	delay(DEPLOYMENT_DELAY);
	
	cout << getTimestamp() << " Configuring pins...\n";
	
	wiringPiSetup();
	
	// Find wPi equivalent of BCM pin numbers
	int linActPin = pinBcmToWpi(LIN_ACT_PIN_BCM);
	int dcMotorSpdPin = pinBcmToWpi(DC_MOTOR_SPD_PIN_BCM);
	int dcMotorIn1Pin = pinBcmToWpi(DC_MOTOR_IN1_PIN_BCM);
	int dcMotorIn2Pin = pinBcmToWpi(DC_MOTOR_IN2_PIN_BCM);
	int rollPin = pinBcmToWpi(ROLL_PIN_BCM);
	int pitchPin = pinBcmToWpi(PITCH_PIN_BCM);
	int yawPin = pinBcmToWpi(YAW_PIN_BCM);
	
	// Set pins to output
	pinMode(linActPin, OUTPUT);
	pinMode(dcMotorSpdPin, OUTPUT);
	pinMode(dcMotorIn1Pin, OUTPUT);
	pinMode(dcMotorIn2Pin, OUTPUT);
	pinMode(rollPin, OUTPUT);
	pinMode(pitchPin, OUTPUT);
	pinMode(yawPin, OUTPUT);
	
	// Start Direwolf
	// ISAAC - Possible solutions if Systemd continues to fail
	//system("./dw-start.sh"); //uncomment later if next line doesnt work
	//system("lxterminal -e direwolf");
	//system("direwolf");
	
	// Deploy the payload
	cout << getTimestamp() << " Extending linear actuator...\n";
	extendLinearActuator(linActPin);
	
	cout << getTimestamp() << " Straightening servos...\n";
	rotateServo60(yawPin, true);
	setServoAngle(pitchPin, PITCH_DEPLOY);
	setServoAngle(rollPin, ROLL_DEPLOY);
	
	cout << getTimestamp() << " Running DC motor to raise payload...\n";
	raisePayload(pinBcmToWpi(DC_MOTOR_SPD_PIN_BCM),
		pinBcmToWpi(DC_MOTOR_IN1_PIN_BCM),
		pinBcmToWpi(DC_MOTOR_IN2_PIN_BCM));
	
	cout << getTimestamp() << " Orienting camera to be even with the horizon...\n";
	orientCamera(pinBcmToWpi(ROLL_PIN_BCM), pinBcmToWpi(PITCH_PIN_BCM));
	
	cout << getTimestamp() << " Awaiting receival of command transmission...\n";
	string transmission = retrieveTransmission();
	cout << getTimestamp() << " Transmission received!\n";
	
	// Loop through each command from the transmission and execute them
	cout << getTimestamp() << " Beginning execution of commands.\n";
	int commandCount = (transmission.length() - 8) / 3;
	float yawAngle = 0;
	for (int c = 0; c < commandCount; c++)
	{
		string command;
		command = transmission.substr(9 + c * 3, 2); //ERROR LINE - This line caused the first 3 commands to be ignored
		
		cout << getTimestamp() << " Executing command " << command << "...\n";
		executeCommand(command, yawPin, yawAngle);
	}
	cout << getTimestamp() << " Execution of all commands completed!\n";
	
	cout << getTimestamp() << " Lowering payload...\n";
	lowerPayload(pinBcmToWpi(DC_MOTOR_SPD_PIN_BCM),
		pinBcmToWpi(DC_MOTOR_IN1_PIN_BCM),
		pinBcmToWpi(DC_MOTOR_IN2_PIN_BCM));
	
	cout << getTimestamp() << " Program terminated.\n";
	return 0;
}

// Returns a string with the current date and time
string getTimestamp()
{
	time_t rawtime = time(0);
	struct tm * timeInfo = localtime(&rawtime);
	char buffer[22];
	strftime(buffer, 22, "[%Y-%m-%d_%T]", timeInfo);
	return buffer;
}

// Returns a string with the current date and time
string getDirewolfDate()
{
	time_t rawtime = time(0);
	struct tm * timeInfo = gmtime(&rawtime);
	char buffer[22];
	strftime(buffer, 22, "%Y-%m-%d", timeInfo);
	return buffer;
}

// Clamps the value between lower and upper
float clamp(float value, float lower, float upper)
{
	return min(upper, max(lower, value));
}

// Parallel arrays include all GPIO pins
const int GPIO_PIN_WPI_IDS[] = {  0,  1,  2,  3,  4,  5,  6,  7, 15, 16, 21, 22, 23, 24, 25, 26, 27, 28, 29 };
const int GPIO_PIN_BCM_IDS[] = { 17, 18, 27, 22, 23, 24, 25,  4, 14, 15,  5,  6, 13, 19, 26, 12, 16, 20, 21 };
const int GPIO_PIN_COUNT = 17;

// Converts a BCM pin id (what appears on the physical RaspberryPi) to a wPi pin id (what is used in code)
int pinBcmToWpi(int id)
{
	for (int i = 0; i < GPIO_PIN_COUNT; i++)
	{
		if (GPIO_PIN_BCM_IDS[i] == id)
			return GPIO_PIN_WPI_IDS[i];
	}
	return -1;
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

// Reads an axis (x, y, or z) from the accelerometer
float readAccelerometerAxis(char axis)
{
	try
	{
		system("./repacc.sh");
		
		ifstream inData;
		inData.open("acc.log");
	
		if (inData.is_open())
		{
			string axisRaw;
		
			inData >> axisRaw;
		
			if (axis == 'x')
				return stof(axisRaw);
		
			inData >> axisRaw;
		
			if (axis == 'y')
				return stof(axisRaw);
		
			inData >> axisRaw;
		
			if (axis == 'z')
				return stof(axisRaw);
		}
	
		inData.close();
	}
	catch (...)
	{
		cout << getTimestamp() << " ERROR: Accelerometer Disconnected\n";
	}
	
	return 0;
}

// Rotates the yaw servo 60 degrees
void rotateServo60(int yawPin, bool clockwise)
{
	// Times are in microseconds
	const int CYCLE_LENGTH = 3000;
	const int CYCLE_COUNT = 28;
	const int ADJUSTMENT_TIME = 1500000;
	const int PULSE_WIDTH_CW = 1300;
	const int PULSE_WIDTH_CCW = 1500;
	
	int pulseWidth = clockwise ? PULSE_WIDTH_CW : PULSE_WIDTH_CCW;
	
	int pulseDelay = CYCLE_LENGTH - pulseWidth;
	
	// Use PWM to rotate 60 degrees
	for (int i = 0; i < CYCLE_COUNT; i++)
	{
		digitalWrite(yawPin, HIGH);
		
		delayMicroseconds(pulseWidth);
	
		digitalWrite(yawPin, LOW);
		
		delayMicroseconds(pulseDelay);
	}
	
	// Delay the remaining amount of the adjustment time
	delayMicroseconds(ADJUSTMENT_TIME - CYCLE_COUNT * CYCLE_LENGTH);
}

// Sets the 180-degree servo at the specified pin to the angle
void setServoAngle(int pin, float angle)
{
	// Angles are in degrees, times are in microseconds
	const int SERVO_RANGE = 180;
	const int PULSE_WIDTH_MIN = 500;
	const int PULSE_WIDTH_MAX = 2500;
	const int CYCLE_LENGTH = 20000;
	const int ADJUSTMENT_TIME = 1500000;
	
	int pulseWidth = PULSE_WIDTH_MIN + (PULSE_WIDTH_MAX - PULSE_WIDTH_MIN) * (angle / SERVO_RANGE);
	int delayTime = CYCLE_LENGTH - pulseWidth;
	
	// Use PWM to rotate roll servo
	for (int i = 0; i < ADJUSTMENT_TIME / CYCLE_LENGTH; i++)
	{
		digitalWrite(pin, HIGH);
		
		// Range: (0) 500-2500 (180)
		delayMicroseconds(pulseWidth);
	
		digitalWrite(pin, LOW);
		
		// Range: (180) 17500-19500 (0)
		delayMicroseconds(delayTime);
	}
}

// Extends the linear actuator to open up the payload compartment
void extendLinearActuator(int pin)
{
	// Times are in microseconds
	const int PULSE_WIDTH_MAX = 2000;
	const int EXTENSION_TIME = 30000000; // 30 Seconds
	int extensionCycleLength = PULSE_WIDTH_MAX * 5;
	int extensionPulses = EXTENSION_TIME / extensionCycleLength;
	for (int p = 0; p < extensionPulses; p++)
	{
		digitalWrite(pin, HIGH);
		
		delayMicroseconds(PULSE_WIDTH_MAX);
	
		digitalWrite(pin, LOW);

		delayMicroseconds(PULSE_WIDTH_MAX * 4);
	}
	
	return;
}

// Runs a DC motor to raise the payload out of the rocket
void raisePayload(int speedPin, int in1Pin, int in2Pin)
{
	// Time in milliseconds
	const int DC_MOTOR_DURATION = 28000; // 28 Seconds
	
	// 1 low 2 high = reverse, 1 high 2 low = forward, same = off
	digitalWrite(in1Pin, HIGH);
	digitalWrite(in2Pin, LOW);
	
	// Set the motor to full speed for the duration
	digitalWrite(speedPin, HIGH);
	delay(DC_MOTOR_DURATION);
	digitalWrite(speedPin, LOW);
	
	// Turn input pins off
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, LOW);
	
	return;
}

// Runs a DC motor to lower the payload into the rocket
void lowerPayload(int speedPin, int in1Pin, int in2Pin)
{
	// Time in milliseconds
	const int DC_MOTOR_DURATION = 28000; // 28 Seconds
	
	// 1 low 2 high = reverse, 1 high 2 low = forward, same = off
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, HIGH);
	
	// Set the motor to full speed for the duration
	digitalWrite(speedPin, HIGH);
	delay(DC_MOTOR_DURATION);
	digitalWrite(speedPin, LOW);
	
	// Turn input pins off
	digitalWrite(in1Pin, LOW);
	digitalWrite(in2Pin, LOW);
	
	return;
}

// Orients the camera to its starting position utilizing the roll and pitch servos
void orientCamera(int rollPin, int pitchPin)
{	
	// Orient roll servo to default position
	cout << getTimestamp() << " Orienting roll servo to default position...\n";
	setServoAngle(rollPin, ROLL_DEFAULT);
	
	// Read from the accelerometer
	float y = readAccelerometerAxis('y');
	float z = readAccelerometerAxis('z');
	
	// Calculate pulse width to correct roll
	float rollOffset = getAngle(-z, y);
	float targetRoll = clamp(ROLL_DEFAULT - rollOffset, ROLL_MIN, ROLL_MAX);
	cout << getTimestamp() << " Roll offset: " << rollOffset << " - Target roll servo angle: " << targetRoll << endl;
	setServoAngle(rollPin, targetRoll);
	
	// Orient pitch servo to default position
	cout << getTimestamp() << " Orienting pitch servo to default position...\n";
	setServoAngle(pitchPin, PITCH_DEFAULT);
	
	// Read from the accelerometer
	float x = readAccelerometerAxis('x');
	z = readAccelerometerAxis('z');
	
	// Calculate pulse width to correct pitch
	float pitchOffset = getAngle(x, z);
	float targetPitch = clamp(PITCH_DEFAULT - pitchOffset, PITCH_MIN, PITCH_MAX);
	cout << getTimestamp() << " Pitch offset: " << pitchOffset << " - Target pitch servo angle: " << targetPitch << endl;
	setServoAngle(pitchPin, targetPitch);
	
	return;
}

// Waits for and retrieves the transmission
string retrieveTransmission()
{
	// Time in milliseconds
	const int TRANSMISSION_CHECK_DELAY = 1000;
	int iteration = 0;
	
	// The callsign from which a transmission will be accepted
	const string CALLSIGN = "KQ4CTL-6"; //"KQ4CTL-6"
	
	string receivedCallsign = "";
	string str;
	int comm = 0;
	
	system(("touch " + getDirewolfDate() + ".log").c_str());
	/*ERROR LINE - this line caused a file with no read, write permissions to be created, cannot log.
	  "chmod" could have voided this error*/
	while (receivedCallsign != CALLSIGN)
	{
		delay(TRANSMISSION_CHECK_DELAY);
		iteration++;
		char stop;
		bool no = true;
		ifstream direlog;
		direlog.open(getDirewolfDate() + ".log");
		while(!direlog.eof())
		{	
			receivedCallsign = "";
			getline(direlog, str);
			comm = 0;
			for (int i = 0; i < str.length(); i++)
			{
				if (comm <= 3)
				{
					if(str[i] == ',')
					{
						comm++;
					}
				}
				else // Is callsign
				{
					if (str[i] == ',')
						break;
					receivedCallsign += str[i];
				}	
			}
			
			// Check for correct callsign
			if (receivedCallsign == CALLSIGN)
				break;
		}
		direlog.close();
		
		if (iteration >= 600) //FAILSAFE - If transmission not receieved within 10 minutes, assume radio failure
		{
			return "C3 A1 D4 C3 E5 A1 G7 C3 H8 A1 F6 C3";
		}
	}
	
	string transmission = "";
	comm = 0;
	for (int i = 0; i < str.length(); i++)
	{
		if (comm <= 20)
		{
			if(str[i] == ',')
			{
				comm++;
			}
		}
		else // Is comment
		{
			if (str[i] == '_')
				break;
			transmission += str[i];
		}	
	}
	
	return transmission;
}


const string CODE_TURN_CAM_RIGHT = "A1";   // Turn camera 60 degrees to the right
const string CODE_TURN_CAM_LEFT = "B2";    // Turn camera 60 degrees to the left
const string CODE_TAKE_PICTURE = "C3";     // Take picture
const string CODE_GRAYSCALE_MODE = "D4";   // Change camera mode from color to grayscale
const string CODE_COLOR_MODE = "E5";       // Change camera mode back from grayscale to color
const string CODE_ROTATE_IMAGE_180 = "F6"; // Rotate image 180 degrees (upside down)
const string CODE_SPECIAL_FILTER = "G7";   // Special effects filter (apply any filter or image distortion you want and state what filter or distortion was used)
const string CODE_REMOVE_FILTERS = "H8";   // Remove all filters

// All times in milliseconds (added 0.2 seconds from expected finish times)
const int PICTURE_DELAY = 6500; // Delay after a picture to allow processing time
const int ROTATE_DELAY = 1300; // Delay after applying a filter to allow processing time
const int FILTER_DELAY = 600; // Delay after applying a filter to allow processing time

// Global boolean values for camera filters (using imageMagick)
bool grayScaleF = false;
bool rotatedF = false;
bool specialF = false;
 
void executeCommand(string command, int yawPin, float &yawAngle)
{
	if (command == CODE_TURN_CAM_RIGHT)
	{
		if (yawAngle + 60 > 180)
		{
			for (int i = 0; i < 5; i++)
				rotateServo60(yawPin, false);
			yawAngle -= 300;
			cout << getTimestamp() << " Camera turned left 300 degrees.\n";
		}
		else
		{
			rotateServo60(yawPin, true);
			yawAngle += 60;
			cout << getTimestamp() << " Camera turned right 60 degrees.\n";
		}
	}
	else if (command == CODE_TURN_CAM_LEFT)
	{
		if (yawAngle - 60 < -180)
		{
			for (int i = 0; i < 5; i++)
				rotateServo60(yawPin, true);
			yawAngle += 300;
			cout << getTimestamp() << " Camera turned right 300 degrees.\n";
		}
		else
		{
			rotateServo60(yawPin, false);
			yawAngle -= 60;
			cout << getTimestamp() << " Camera turned left 60 degrees.\n";
		}
	}
	else if (command == CODE_TAKE_PICTURE)
	{
		// Take a picture with the timestamp as its name
		string now = getTimestamp();
		system(("touch " + now + ".jpg").c_str());
		system("raspistill -n --width 960 --height 720 -o temp.jpg");
		system(("mv temp.jpg " + now + ".jpg").c_str());
		//delay(PICTURE_DELAY);
		
		// Apply filters
		if(grayScaleF)
		{
			system(("convert " + now + ".jpg -colorspace LinearGray " + now + ".jpg").c_str());
			//delay(FILTER_DELAY);
		}
		if(rotatedF)
		{
			system(("convert " + now + ".jpg -distort SRT 180 " + now + ".jpg").c_str());
			//delay(ROTATE_DELAY);
		}
		if(specialF)
		{
			system(("convert " + now + ".jpg -brightness-contrast 50% " + now + ".jpg").c_str());
			//delay(FILTER_DELAY);
		}
		system(("convert " + now + ".jpg -pointsize 28 -fill black -annotate +25+700 " + now + " " + now + ".jpg").c_str());
		
		cout << getTimestamp() << " Picture taken.\n";
	}
	else if (command == CODE_GRAYSCALE_MODE)
	{
		grayScaleF = true;
		cout << getTimestamp() << " Grayscale mode activated.\n";
	}
	else if (command == CODE_COLOR_MODE)
	{
		grayScaleF = false;
		cout << getTimestamp() << " Color mode activated.\n";
	}
	else if (command == CODE_ROTATE_IMAGE_180)
	{
		rotatedF = !rotatedF;
		cout << getTimestamp() << " Increased image rotation by 180 degrees.\n";
	}
	else if (command == CODE_SPECIAL_FILTER)
	{
		specialF = true;
		cout << getTimestamp() << " Applied brightness filter.\n";
	}
	else if (command == CODE_REMOVE_FILTERS)
	{
		grayScaleF = false;
		specialF = false;
		rotatedF = false;
		cout << getTimestamp() << " Removed all filters.\n";
	}
	
	return;
}
