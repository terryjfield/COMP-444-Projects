/*
  Terry Field, Student # 3721738
  COMP 444 
  April 12th, 2025


*/
#include <SerialMessageHandler.h>                                                                                        
#include <NewPing.h>
#include <SoftwareSerial.h>

// Constants for digital I/O pins
const byte FRONT_PIN = 3;                  
const byte LEFT_PIN  = 5;
const byte RIGHT_PIN = 4;
const byte BACK_PIN  = 6;

const byte LED_PIN = 11;

// Dimensions of the arena, in centimeters
const int ARENA_LENGTH = 151;
const int ARENA_WIDTH  = 75;
const int ARENA_WIDTH_TOLERANCE  = 1; // Tolerance in centimeters when reading width, used in positioning.
const int ARENA_LENGTH_TOLERANCE = 0;

// Offsets to the sensor (cm) to the middle of the robot used as the robot's location
const int FRONT_SENSOR_OFFSET = 6;
const int BACK_SENSOR_OFFSET  = 8;
const int RIGHT_SENSOR_OFFSET = 4;
const int LEFT_SENSOR_OFFSET  = 4;

// Distance between sensors on robot
const int LEFT_RIGHT_GAP = 5;
const int FRONT_BACK_GAP = 15;

bool safetyStopMode = false; // if set, the robot should halt

// Global variables containing current speed
int rightSpeed = 0;
int leftSpeed = 0;

// Indicator LED
bool ledOn = false;

// Instantiate NewPing objects for the 4 ultrasonic sensors
const int MAX_DISTANCE = 400; // Readings greater than 400 will be discarded by the NewPing library
NewPing leftSonar(LEFT_PIN, LEFT_PIN, MAX_DISTANCE);
NewPing rightSonar(RIGHT_PIN, RIGHT_PIN, MAX_DISTANCE);
NewPing frontSonar(FRONT_PIN, FRONT_PIN, MAX_DISTANCE);
NewPing backSonar(BACK_PIN, BACK_PIN, MAX_DISTANCE);

// Serial comms to Zumo Arduino
const byte zumoRx = 10;
const byte zumoTx = 9;
SerialMessageHandler smh(zumoRx,zumoTx,Serial,true);

// BlueTooth radio pins
const byte bluetoothTx = 7;  // TX-O pin of bluetooth mate, Arduino D6
const byte bluetoothRx = 8;  // RX-I pin of bluetooth mate, Arduino D7

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void bluetoothSetup() {
  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
}

void setup() {
  bool ledOn = false;

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  bluetoothSetup();
  // Flash the LED for 10 seconds to indicate that we're waiting for the user to connect to the Bluetooth radio
  for (int i = 0; i < 10; i++) {
    if (ledOn)
      digitalWrite(LED_PIN, LOW);
    else    
      digitalWrite(LED_PIN, HIGH);
    ledOn = !ledOn;
    serialPrintln("Waiting...");
    delay(1000);    
  }
}

// Main loop
void loop() {
  long front,back,right,left;
  int currentX,currentY;

  if (!safetyStopMode) { // The robot will halt if any sensor comes within 3.0 cm of an object
    long left,right,front,back;
    getSonarReadings(&left, &right, &front, &back);                  
    serialPrintln("L: " + String(left) + " R: " + String(right) + " F: " + String(front) + " B: " + String(back));
    if (dangerClose(left, right, front, back)) { // Check if we are close to an object
      stopMotors();
      serialPrintln("Safety stop mode engaged.");
      safetyStopMode = true;
    } else { // Not danger close
      navigateToCoordinate(75,45);
      safetyStopMode = true;
    }
  } else {
    if (ledOn)
      digitalWrite(LED_PIN, LOW);
    else    
      digitalWrite(LED_PIN, HIGH);
    ledOn = !ledOn;
    delay(500);
  }
}

/**
  This method sends the command string to the Zumo controller and returns a boolean indicating if
  the message was successfully received.
*/
bool sendCommandToZumo(String commandStr) {
  bool messageToSend = true;
  protoState msgState = READY;
  bool result = false;

  serialPrintln("Sending command: " + commandStr);
  smh.listen(); // The SoftwareSerial library can only "listen" or receive characters on one port. We have two in this project.
  
  do { // iterate until the message state is one of the three terminal states below.
    smh.sendMessage(&msgState, commandStr);
    switch (msgState) {
      case ACK_RECEIVED:
        Serial.println("ACK received, message sent");
        msgState = READY;
        messageToSend = false;
        result = true;
        break;
      case ERROR:
        Serial.println("ERROR in transmission");
        msgState = READY;
        messageToSend = false;
        break;
      case TIMEOUT:
        Serial.println("Timeout");
        msgState = READY;
        messageToSend = false;
        break;
      }
  } while (messageToSend == true);
  return result;
}

/**
  Check to see if any of the sonar sensors has a close reading, i.e 3.0 cm or less. If so,
  return true, else return false.
*/
bool dangerClose(long left, long right, long front, long back) {
  if ((left <= 3 && left > 0 )|| (right <= 3 && right > 0) || (front <= 3 && front > 0) || (back <= 3 && back > 0)) 
    return true;
  else
    return false;
}

/**
  Reads all four sonar sensors.
*/
void getSonarReadings(long *left, long *right, long *front, long *back) {
  delay(29);
  *left = getDistance(leftSonar);
  delay(29);
  *right = getDistance(rightSonar);
  delay(29);
  *back = getDistance(backSonar);
  delay(29);
  *front = getDistance(frontSonar);
}

/**
  This method reads only two sensors, the left and right, so that it is faster than reading
  all four sensors.
*/
long getLeftRightDistance() {
  long left, right;

  delay(29);
  left = getDistance(leftSonar);
  delay(29);
  right = getDistance(rightSonar);
  return left + right + LEFT_RIGHT_GAP;
}

/**
  This method reads the sonar sensors and determines if the robot is orientated parallel to a set of walls. 
*/
bool isParallelToWall(long *leftRightDistance, int arenaLength, int arenaTolerance) {
  long left,right;

  *leftRightDistance = getLeftRightDistance();
  serialPrintln("Left-right distance: " + String(*leftRightDistance));
  if (((arenaLength - arenaTolerance) <= *leftRightDistance) && (*leftRightDistance <= (arenaLength + arenaTolerance)))
    return true;
  else
    return false;
}

void setSpeed(int leftMotor, int rightMotor) {
  rightSpeed = rightMotor;
  leftSpeed  = leftMotor;
  char message[15];
  sprintf(message, "S%04d,%04d", leftMotor, rightMotor);
  sendCommandToZumo(message);
}

// Sends command to stop both left and right motors
void stopMotors() {
  setSpeed(0,0); 
}

void rotateClockwise(int speed) {
  setSpeed(speed,-1*speed);
}

void rotateCounterClockwise(int speed) {
  setSpeed(-1*speed, speed);
}

/**
  This method moves the robot so that it is orientated at zero degrees.
*/
void moveToZeroUpPos(bool clockwise) {
  long leftRightDistance;
  bool motorsInMotion = false;

  while (!isParallelToWall(&leftRightDistance, ARENA_LENGTH, ARENA_LENGTH_TOLERANCE)) {
    if (!motorsInMotion) {
      motorsInMotion = true; 
      if (clockwise)
        rotateClockwise(120);
      else
        rotateCounterClockwise(120);
    }
  }
  stopMotors();
}

/**
  This method assumes a zero-degree orientation to start with.
*/
void rotateToOrientation(int orientation) {
  long leftRightDistance;
  int speed;
  bool motorsInMotion = false;
  
  switch (orientation) {
    case 90: 
      speed = 120;
      break;
    case 270:
      speed = -120; // CounterClockwise speed
      break;
    default:
      serialPrintln("Invalid orientation:"  + String(orientation));
      return false;
  }

  while (!isParallelToWall(&leftRightDistance, ARENA_WIDTH, ARENA_WIDTH_TOLERANCE)) {
    if (!motorsInMotion) {
      motorsInMotion = true;
      rotateClockwise(speed);   
    }
  }

  stopMotors();
} 


void getCurrentPosition(int *x, int *y, int orientation) {
  long left,right,front,back;

  getSonarReadings(&left, &right, &front, &back);
  switch (orientation) {
    case 0:
    case 180:
      *x = left + LEFT_SENSOR_OFFSET;
      *y = back + BACK_SENSOR_OFFSET;
      break;
    case 90:
    case 270:
      *x = back + BACK_SENSOR_OFFSET;
      *y = left + LEFT_SENSOR_OFFSET;
      break;
    default:
      serialPrintln("Invalid orientation: " + String(orientation));            
  }
}

void moveToYCoord(int coordX, int coordY) {
  int currentX, currentY, displacementX, displacementY;
  int motorSpeed, backDistance;

  serialPrintln("Positioning to Y coordinate");
  getCurrentPosition(&currentX, &currentY, 0);

  // Calculate the x and y displacements to the the target
  displacementX = coordX - currentX;
  displacementY = coordY - currentY;
  serialPrintln("Current location: (" + String(currentX) + "," + String(currentY) + ")");
  serialPrintln("Displacement : (" + String(displacementX) + "," + String(displacementY) + ")");

  // Move to Y position
  if (displacementY < 0) 
    motorSpeed = -120;
  else
    motorSpeed = 120;

  setSpeed(motorSpeed, motorSpeed);
  
  do {
    delay(29);
    backDistance = getDistance(backSonar) + BACK_SENSOR_OFFSET;
    serialPrintln("Back distance = " + String(backDistance));
  } while (backDistance != coordY);

  stopMotors();  
}

void navigateToCoordinate(int coordX, int coordY) {
  int currentX, currentY, displacementX, displacementY;
  int motorSpeed, backDistance;

  moveToZeroUpPos(true);
  moveToYCoord(coordX,coordY);
  moveToZeroUpPos(true);

  // Once we are at the correct distance from the y-axis, the rotate to 90 degrees to move along the x-axis
  rotateToOrientation(90);

  getCurrentPosition(&currentX, &currentY, 90);
  serialPrintln("Current location: (" + String(currentX) + "," + String(currentY) + ")");

  // Calculate the x and y displacements to the the target
  displacementX = coordX - currentX;
  displacementY = coordY - currentY;
  serialPrintln("Current location: (" + String(currentX) + "," + String(currentY) + ")");
  serialPrintln("Displacement : (" + String(displacementX) + "," + String(displacementY) + ")");

  if (displacementX > 0)
    motorSpeed = 120;
  else
    motorSpeed = -120;

  setSpeed(motorSpeed,motorSpeed);

  do {
    delay(29);
    backDistance = getDistance(backSonar) + BACK_SENSOR_OFFSET;
    serialPrintln("Back distance = " + String(backDistance));
  } while (backDistance != coordX);
  
  stopMotors();

  serialPrintln("Completed X translation");
  getCurrentPosition(&currentX, &currentY, 90);
  serialPrintln("Current location: (" + String(currentX) + "," + String(currentY) + ")");

  moveToZeroUpPos(false);
  moveToYCoord(coordX,coordY);
}

// Method to output to both the bluetooth radio and the serial monitor.
template<typename T>
void serialPrint(T value) {
  Serial.print(value);
  bluetooth.print(value);
}

// Method to output to both the bluetooth radio and the serial monitor, with linefeed.
template<typename T>
void serialPrintln(T value) {
  Serial.println(value);
  bluetooth.println(value);
}

// Returns the distance measured by the HC-SR04 distance sensor
long getDistance(NewPing &sonar)
{
  return sonar.ping_cm();
}
