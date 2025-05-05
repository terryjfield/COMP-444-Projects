// Green board
#include "SerialMessageHandler.h"                                                                                            "

String receivedMessage;
protoState msgState = READY;

SerialMessageHandler smh(10,11,Serial);

void setup() {
  Serial.begin(9600);
  smh.debugMessage("hey!");
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
    delay(200);
}

void setMotorSpeeds(int leftMotorSpeed, int rightMotorSpeed) {
  Serial.print("Left motor: ");
  Serial.print(leftMotorSpeed);
  Serial.print(" Right motor: ");
  Serial.println(rightMotorSpeed);
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
      sscanf(commandStr.c_str(), "S%3d,%3d", &leftMotorSpeed, &rightMotorSpeed);
      setMotorSpeeds(leftMotorSpeed, rightMotorSpeed);
      break;
  }
}

