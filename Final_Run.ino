#include "Servo.h"
#include "math.h"
//servo
#define servoPin 9
//fans
#define fanLiftPin 6
#define fanThrustPin 10
//sensors
#define sensorUltraPin 3
#define sensorInfraPin 3

volatile float ultra, infra, mmInfra, mmUltra;
volatile float angle = 0.0;
int counter = 0;

Servo myServo;

// Reads ultrasonic sensor
int readUltra()
{
  ultra = pulseIn(3, HIGH);
  mmUltra = ultra/5.8;
  return mmUltra;
}

//Reads infraread sensor
int readInfra()
{
  infra = analogRead(3);
  mmInfra = (106500.8 * pow(infra,-0.935))-75;
  return mmInfra;
}

//Gets the % difference between the two measured distances
//Returns a % of 90 degrees (from 0 - 180, but we scale to -90 to 90)
float getAngle()
{
  float Inf = readInfra();
  float Ult = readUltra();

  //If conditions exclude extreme (3 meter+) measurements as a result of discrepancies
  //if (Inf < 3000|| Ult>3000)
  //{
  //  return -1;
  //}
  //else
  //{
  angle = 90 -  60* (Inf-Ult)/((Inf+Ult)/2);
  return angle;
  //}
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //articulation test
  pinMode(3, INPUT);
  myServo.attach(9);
  myServo.write(90);
  delay(2000);
  myServo.write(180);
  delay(500);
  myServo.write(90); 
  delay(250);
  myServo.write(0);
  delay(250);
  myServo.write(180);
  delay(500);
  myServo.write(0);
  delay(1000);
  myServo.write(90);
  delay(500);
  digitalWrite(fanLiftPin,1);
  digitalWrite(fanThrustPin,1);
}

void loop() {
  // put your main code here, to run repeatedly:
   angle = getAngle();
   //digitalWrite(fanThrustPin,1);
   Serial.print("Angle");
   Serial.println(angle);
   Serial.print("Infra: ");
   Serial.println(mmInfra);
   Serial.print("Ultra: ");
   Serial.println(mmUltra);
   if (angle != -1)
   {
    //delay(100)
    myServo.write(angle);
    /*if (abs(90-angle) > 60)
    {
      digitalWrite(fanLiftPin,0);
    }*/
   }
   else 
   {
   Serial.print("Error detected: ");
   Serial.println(++counter);
   }
   //delay(500);
}
