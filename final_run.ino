// AUTHORS, EWAY-FRANCIS
#include "Servo.h"
#include "math.h"


// DEFINING PINS TO BE USED
#define fanLiftPin 10
#define fanThrustPin 6
#define sensorUltraPin 3
#define sensorInfraPin 3

#define scale 80
#define SIZE 3
#define maxAngle 90

volatile float ultra, infra, mmInfra, mmUltra;
volatile float angle = 0.0;
static float minAngle = 9;

// DEFINING THE ARRAY TO HOLD SIZE READINGS FROM THE SENSORS
float infraReadings[SIZE] = {0};
float ultraReadings[SIZE] = {0};

// LOOP RELATED VARIABLES
int pos = 0;
int sumInfra = 0;
int sumUltra = 0;
bool turning = false;

float previousAngle = 0;

// INTIALIZING SERVO OBJECT
Servo myServo;

// FUNCTIONs TO READ A VALUE FROM THE ULTRASONIC SENSOR and INFRARED SENSOR
int readUltra()
{
  ultra = pulseIn(3,HIGH);
  mmUltra = ultra / 5.8;
  return mmUltra;
}

int readInfra()
{
  infra = analogRead(3);
  mmInfra = (106500.8 * pow(infra, -0.935)) - 75;
  return mmInfra;
}

// FUNCTION TO COMPUTE THE ANGLE BASED ON SUM OF READINGS IN
//   infraReadings AND  ultraReadings

float getAngle(float x, float y)
{
  float inf = x;
  float ult = y;
  float check = 0;

  float trueAngle = scale * (pow((ult - inf) / ((ult + inf)/2),2));

  // CHECK IF THE DISTANCE TO THE LEFT SIDE IS BIGGER
  // IF YES, MAKE THE ANGLE NEGATIVE, ADD AN OFFSET OF 30
  if ((ult - inf) < 0)
  {
    trueAngle = trueAngle * -1;
    trueAngle = trueAngle - 30;
  }
  else
  {
    trueAngle = trueAngle + 60;
  }
  
  if (trueAngle > maxAngle)
  {
    trueAngle = maxAngle;
  }

  if (trueAngle < -1 * maxAngle)
  {
    trueAngle = -1 * maxAngle;
  }

  if (abs(trueAngle) < minAngle)
  {
    trueAngle = 0;
  }

  angle = 90 - trueAngle;
  return angle;
} // END

bool isTurn(int x, int y)
{
  int sum = x + y;
  if (sum > 525) return true;
  return false; 
} // END

void turnFan(float angle, float previousAngle)
{
  if (angle < previousAngle + 1) myServo.write(angle);

  if(angle > previousAngle - 1)
  {
    for (int i = previousAngle; i <= int(angle); i++)
    {
      delay(3);
      myServo.write(i);
    } // END FOR LOOP
  } // END if (angle > previousAngle -1)
} // END 



void setup() {
  // SETTING UP THE PINS
  Serial.begin(9600);
  pinMode(3, INPUT);
  myServo.attach(9);
  myServo.write(90);
  digitalWrite(fanLiftPin, 1);
  digitalWrite(fanLiftPin, 1);
}

void loop() {
  infraReadings[pos] = readInfra();
  ultraReadings[pos] = readUltra();

  if (ultraReadings[pos] > 3000 || infraReadings[pos] > 3000)
  {
    goto skip;
  } //END

  for (int i = 0; i < SIZE; i++)
  {
    sumInfra = sumInfra + infraReadings[i];
    sumUltra = sumUltra + ultraReadings[i];
  } // END

  angle = getAngle(sumInfra, sumUltra);
  turning = isTurn(sumInfra / SIZE, sumUltra / SIZE);

  if (turning == true)
  {
    turnFan(angle, previousAngle);
  } // END

  else if (turning == false)
  {
    myServo.write(70);
  } // END

  sumUltra = 0;
  sumInfra = 0;
  previousAngle = angle;
  pos++;
  pos = pos % SIZE;
skip:
  {
    // SKIP
  }
} // END LOOP
