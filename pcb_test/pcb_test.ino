/*
  Terry Field, Student # 3721738
  COMP 444 
  April 12th, 2025


*/
#include <SerialMessageHandler.h>                                                                                        
#include <NewPing.h>
#include <SoftwareSerial.h>

const byte FRONT_PIN = 3;                  
const byte LEFT_PIN  = 5;
const byte RIGHT_PIN = 4;
const byte BACK_PIN  = 6;

const byte LED_PIN = 11;

const int MAX_DISTANCE = 400;

// Instantiate NewPing objects for the 4 ultrasonic sensors
NewPing leftSonar(LEFT_PIN, LEFT_PIN, MAX_DISTANCE);
NewPing rightSonar(RIGHT_PIN, RIGHT_PIN, MAX_DISTANCE);
NewPing frontSonar(FRONT_PIN, FRONT_PIN, MAX_DISTANCE);
NewPing backSonar(BACK_PIN, BACK_PIN, MAX_DISTANCE);

// Serial comms to Zumo Arduino
const byte zumoRx = 10;
const byte zumoTx = 9;
SerialMessageHandler smh(zumoRx,zumoTx,Serial,true);

// BlueTooth
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
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  bluetoothSetup();
  delay(10000);
  digitalWrite(LED_PIN, HIGH);
  rotationTest();
  digitalWrite(LED_PIN, LOW);
}

void getSonarReadings(float *left, float *right, float *front, float *back) {
  delay(29);
  *left = getDistance(leftSonar);
  delay(29);
  *right = getDistance(rightSonar);
  delay(29);
  *back = getDistance(backSonar);
  delay(29);
  *front = getDistance(frontSonar);
}


bool sendCommandToZumo(String commandStr) {
  bool messageToSend = true;
  protoState msgState = READY;
  bool result = false;

  smh.listen();
  
  do {
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

String message1 = "S0050,-0050";
String message2 = "S0100,-100";
String message3 = "S0150,-150";
String message4 = "S200,-200";
String message5 = "S0000,0000";

void oneSecondStop() {
  serialPrintln(message5);
  sendCommandToZumo(message5);
  delay(1000);  
}
void rotationTest() {
  serialPrintln(message1);
  sendCommandToZumo(message1);
  delay(2000);
  oneSecondStop();
  serialPrintln(message2);
  sendCommandToZumo(message2);
  delay(2000);
  oneSecondStop();
  serialPrintln(message3);
  sendCommandToZumo(message3);
  delay(2000);
  oneSecondStop();
  serialPrintln(message4);
  sendCommandToZumo(message4);
  delay(2000);
  serialPrintln(message5);
  sendCommandToZumo(message5);
}

void loop() {
  float left,right,front, back;
  digitalWrite(LED_PIN, HIGH);
  getSonarReadings(&left, &right, &front, &back);                  
  serialPrintln("L: " + String(left) + " R: " + String(right) + " F: " + String(front) + " B: " + String(back));
  sendCommandToZumo(message5);
  delay(200);
}

template<typename T>
void serialPrint(T value) {
  Serial.print(value);
  bluetooth.print(value);
}

template<typename T>
void serialPrintln(T value) {
  Serial.println(value);
  bluetooth.println(value);
}

// Returns the distance measured by the HC-SR04 distance sensor
float getDistance(NewPing &sonar)
{
  return sonar.ping_cm();
}
