/*
  SparkFun Inventor’s Kit
  Circuit 3B-Distance Sensor

  Control the color of an RGB LED using an ultrasonic distance sensor.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <Servo.h>                //include the Servo library

const int trigPin = 11;           //connects to the trigger pin on the distance sensor
const int echoPin = 12;           //connects to the echo pin on the distance sensor

const int redPin    = 3;           //pin to control the red LED inside the RGB LED
const int greenPin  = 5;           //pin to control the green LED inside the RGB LED
const int bluePin   = 6;           //pin to control the blue LED inside the RGB LED
const int servoPin  = 9;           //pin to control the servo
const int buzzerPin = 10;          //pin to control the buzzer

float distance = 0;               //stores the distance measured by the distance sensor

Servo myServo; // create a servo object

void setup()
{
  Serial.begin (9600);        //set up a serial connection with the computer

  pinMode(trigPin, OUTPUT);   //the trigger pin will output pulses of electricity
  pinMode(echoPin, INPUT);    //the echo pin will measure the duration of pulses coming back from the distance sensor

  //set the RGB LED pins to output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(buzzerPin, OUTPUT); // set the buzzer to output

  myServo.attach(servoPin); 
}

void loop() {
  distance = getDistance();   //variable to store the distance measured by the sensor

  Serial.print(distance);     //print the distance that was measured
  Serial.println(" cm");      //print units after the distance

  if (distance <= 5) {                       //if the object is close

    //make the RGB LED red
    analogWrite(redPin, 255);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

    tone(buzzerPin, 272);
    myServo.write(180);
    delay(350); // need to give the servo time to move

    noTone(buzzerPin);
    myServo.write(0);
    delay(350);  // need to give the servo time to move

  } else if ((distance > 5) && (distance <= 10)) { //if the object is a medium distance

    //make the RGB LED yellow
    analogWrite(redPin, 255);
    analogWrite(greenPin, 50);
    analogWrite(bluePin, 0);

  } else if ((distance > 10) && (distance <= 15)) {                                    //if the object is far away

    //make the RGB LED blue
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 255);

  } else {

    //make the RGB LED green
    analogWrite(redPin, 0);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 0);
  }

  delay(200);      //delay 100ms between each reading
}

//------------------FUNCTIONS-------------------------------

//RETURNS THE DISTANCE MEASURED BY THE HC-SR04 DISTANCE SENSOR
float getDistance()
{
  float echoTime;                   //variable to store the time it takes for a ping to bounce off an object
  float calculatedDistance;         //variable to store the distance calculated from the echo time

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);      //use the pulsein command to see how long it takes for the
                                          //pulse to bounce back to the sensor

  // The speed of sound is 343 meters/second or 34,300 cm/s. The constant 0.01715 = (34300 cm/s * 0.5) / 1000000 microseconds/second
  calculatedDistance = echoTime * 0.01715;
  
  return calculatedDistance;              //send back the distance that was calculated
}

