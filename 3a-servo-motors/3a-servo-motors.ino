/*
  Terry Field, Student # 3721738
  COMP 444 
  March 1st, 2025

  This sketch implements the coding challenges for Project 2B of the SparkFun Inventor's Kit
  guide. 

*/

#include <Servo.h> // include the Servo library

int potPosition;   // potentiometer position read from the analog input
int servoPosition; // servo will move to this position

Servo myservo;  // instantiate a servo object


void setup() {
  myservo.attach(9);  // associate pin 9 with the servo
}

void loop() {
  
  potPosition = analogRead(A0); // read the potentiometer position

  servoPosition = map(potPosition, 0, 1023, 100, 50);

  myservo.write(servoPosition);

}
