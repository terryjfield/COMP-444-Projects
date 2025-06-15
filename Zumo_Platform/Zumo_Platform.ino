#include <Wire.h>
#include <ZumoShield.h>
#include "SerialMessageHandler.h"  

ZumoMotors motors;
//Pushbutton button(ZUMO_BUTTON);
ZumoIMU imu;

// For receiving commands
String receivedMessage;
protoState msgState = READY;

SerialMessageHandler smh(5,4,Serial,true);

void setup() {
  Serial.begin(9600);
  smh.debugMessage("hey!");

  // Initialize the Wire library and join the I2C bus as a master
  Wire.begin();

  // Initialize IMU
  imu.init();

  // Enables accelerometer and magnetometer
  imu.enableDefault();
  imu.configureForCompassHeading();
  setMotorSpeeds(0,0);
}

void loop() {
    smh.receiveMessage(&msgState, receivedMessage);
    switch (msgState) {
      case MESSAGE_RECEIVED:
        Serial.print("Message received: ");
        Serial.println(receivedMessage);
        executeCommand(receivedMessage);
        msgState = READY;
        break;
      case ERROR:
        Serial.println("ERROR in transmission");
        msgState = READY;
        break;
      case TIMEOUT:
        Serial.println("Timeout in receiving message");
        msgState = READY;
        break;        
    }
}

void setMotorSpeeds(int leftMotorSpeed, int rightMotorSpeed) {
  Serial.print("Left motor: ");
  Serial.print(leftMotorSpeed);
  Serial.print(" Right motor: ");
  Serial.println(rightMotorSpeed);
  motors.setLeftSpeed(leftMotorSpeed);
  motors.setRightSpeed(-1 * rightMotorSpeed);
}

/**
  This function parse the command string and call the appropriate function.

  Commands are:
    S<left motor speed>,<right motor speed> : Speed control for motors
    C : transmit heading
    T<frequency>,<duration> : Play tone of frequency for duration.

*/
void executeCommand(String commandStr) {
  char command = commandStr[0];
  switch (command) {
    case 'S':
      int leftMotorSpeed, rightMotorSpeed;
      sscanf(commandStr.c_str(), "S%4d,%4d", &leftMotorSpeed, &rightMotorSpeed);
      setMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
      break;
  }
}

