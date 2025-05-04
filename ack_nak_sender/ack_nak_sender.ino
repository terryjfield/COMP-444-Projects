// Blue board
#include <SoftwareSerial.h>

const byte STX = 0x02;
const byte ETX = 0x03;
const byte ACK = 0x06;
const byte NAK = 0x15;

enum protoState { READY,
                  TIMEOUT,
                  ERROR,
                  WAITING_FOR_ACK,
                  ACK_RECEIVED,
                  NAK_RECEIVED,
                  WAITING_FOR_STX,
                  WAITING_FOR_ETX,
                  MESSAGE_RECEIVED 
                  };

const byte NO_CHAR = 0x01;
const byte PROTOCOL_ERROR = 0x00;
const byte MAX_RETRIES = 3;
const unsigned long TIMEOUT_MS = 3000;

// Global variables that hold the state of the message transmission
unsigned long startMillis;  // stores current milliseconds when we started timeout timer
byte retransmitCount;       // stores of times the message has been retried/re-transmitted
byte NAKcount;              // stores the number of times a received message has been NAK'd
protoState msgState = READY;
boolean messageToSend = true;
boolean messageToReceive = false;

String receivedMessage;

const byte RX_PIN = 10;
const byte TX_PIN = 11;

SoftwareSerial slaveSerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
  slaveSerial.begin(9600);
}

void loop() {
  if (messageToSend) {
    sendMessage(&msgState, "ab", slaveSerial, &startMillis);
    switch (msgState) {
      case ACK_RECEIVED:
        Serial.println("ACK received");
        msgState = READY;
        messageToSend = false;
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
  }

  if (messageToReceive) {
    receiveMessage(&msgState, receivedMessage, slaveSerial, &startMillis);
    switch (msgState) {
      case MESSAGE_RECEIVED:
        Serial.print("Message received: ");
        Serial.print(receivedMessage);
        msgState = READY;
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
  }
  delay(50);
}

byte calculateChecksum(String message) {
  int len = message.length();
  byte chksum = 0;
  for (int i = 0; i < len; i++) {
    chksum += message[i];
  }
  return chksum;
}

void transmitFrame(String message, SoftwareSerial &serialPort, unsigned long *timer) {
  String frame;
  byte frameLen;

  byte chksum = calculateChecksum(message);
  frame = (char)STX;
  frame += message;
  frame += (char)chksum;
  frame += (char)ETX;
  frameLen = frame.length();
  for (int i = 0; i < frameLen; i++) {
    serialPort.write(frame[i]);
  }
  *timer = millis();  // capture the time when the message was sent
}

/**
  See if an ACK character has been received.
*/
protoState receiveACK(SoftwareSerial &serialPort, unsigned long timer) {
  byte incomingByte = 0;
  if (serialPort.available() > 0) {
    incomingByte = serialPort.read();
    switch (incomingByte) {
      case ACK:
        return ACK_RECEIVED;
        break;
      case NAK:
        return NAK_RECEIVED;
        break;
      default:
        serialPort.write(NAK);
        serialPort.flush();
        return ERROR;
    };
  } else {
    unsigned long currentMillis;
    currentMillis = millis();
    if (currentMillis - timer >= TIMEOUT_MS) {  // Timeout
      return TIMEOUT;
    }
  }
  return WAITING_FOR_ACK;
}

protoState transmitMessage(String message, SoftwareSerial &serialPort, unsigned long *timer) {
  transmitFrame(message, serialPort, timer);
  return WAITING_FOR_ACK;
}

void sendMessage(protoState *msgState, String message, SoftwareSerial &serialPort, unsigned long *timer) {
  protoState localState;
  localState = *msgState;
  switch (localState) {
    case READY:
      transmitMessage(message, serialPort, timer);
      *msgState = WAITING_FOR_ACK;
      retransmitCount = 0;
      break;
    case WAITING_FOR_ACK:
      *msgState = receiveACK(serialPort, *timer);
      break;
    case NAK_RECEIVED:
      if (retransmitCount == MAX_RETRIES) {
        *msgState = ERROR;
        return;
      } else {
        transmitMessage(message, serialPort, timer);
        *msgState = WAITING_FOR_ACK;
        retransmitCount++;
      }
      break;
  }
}

void transmitByte(byte value, SoftwareSerial &serialPort) {
  serialPort.write(value);
}

void receiveMessage(protoState *msgState, String &message, SoftwareSerial &serialPort, unsigned long *timer) {
  if (NAKcount == MAX_RETRIES) {
    *msgState = ERROR;
  }
  byte incomingByte;
  if (*msgState == READY) { // Set up timer, clear message buffer
    *msgState = WAITING_FOR_STX;
    NAKcount = 0;
    *timer = millis();  // capture the time when we wait for the message
  }
  if (serialPort.available() > 0) {
    incomingByte = serialPort.read();
    switch (*msgState) {
      case WAITING_FOR_STX:
        message = "";
        if (incomingByte != STX) { // first byte must be an STX
          transmitByte(NAK, serialPort);
          NAKcount++;
          serialPort.flush();
          return;
        }
        *msgState = WAITING_FOR_ETX;
        break;
      case WAITING_FOR_ETX:
        if (incomingByte != ETX) {
          message += incomingByte;
        } else { // ETX received
          unsigned int msgLen = message.length();
          byte checksum = message[msgLen - 1];
          byte calculatedChecksum = calculateChecksum(message.substring(0, msgLen - 2));
          if (checksum == calculatedChecksum) { // successfully received message
            transmitByte(ACK, serialPort);
            *msgState = MESSAGE_RECEIVED;
          } else { 
            transmitByte(NAK, serialPort);
            NAKcount++;
            serialPort.flush();
            *msgState = WAITING_FOR_STX;
          }
        }   
    } //switch
  } else { // no character available so lets check if we timeout
    unsigned long currentMillis;
    currentMillis = millis();
    if (currentMillis - *timer >= TIMEOUT_MS) {  // Timeout
      return TIMEOUT;
    }    
  }
}
