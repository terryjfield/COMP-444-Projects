#include "Arduino.h"
#include "SerialMessageHandler.h"

SerialMessageHandler::SerialMessageHandler(int rxPin, int txPin, HardwareSerial &serialPort) {
    this->softSerial = new SoftwareSerial(rxPin, txPin);
    this->softSerial->begin(9600);
    this->hw_serial  = &serialPort;
}

void SerialMessageHandler::debugMessage(String message) {
    this->hw_serial->println(message);
}

byte SerialMessageHandler::calculateChecksum(String message) {
    int len = message.length();
    byte chksum = 0;
    for (int i = 0; i < len; i++) {
      chksum += message[i];
    }
    return chksum;
  }
  
  void SerialMessageHandler::transmitFrame(String message, SoftwareSerial &serialPort, unsigned long *timer) {
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
    serialPort.flush();
    *timer = millis();  // capture the time when the message was sent
  }
  
  /**
    See if an ACK character has been received.
  */
  protoState SerialMessageHandler::receiveACK(SoftwareSerial &serialPort, unsigned long timer) {
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
  
  protoState SerialMessageHandler::transmitMessage(String message, SoftwareSerial &serialPort, unsigned long *timer) {
    transmitFrame(message, serialPort, timer);
    return WAITING_FOR_ACK;
  }
  
  void SerialMessageHandler::sendMessage(protoState *msgState, String message, SoftwareSerial &serialPort, unsigned long *timer) {
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
  
  void SerialMessageHandler::transmitByte(byte value) {
    this->softSerial->write(value);
  }
  
  void SerialMessageHandler::receiveMessage(protoState *msgState, String &message) {
    
    byte incomingByte;
    if (*msgState == READY) { // Set up timer, clear message buffer
      *msgState = WAITING_FOR_STX;
      this->NAKcount = 0;
      this->timer = millis();  // capture the time when we wait for the message
      this->debugMessage("Waiting for STX");
    }
    if (this->softSerial->available() > 0) {
      incomingByte = this->softSerial->read();
      this->debugMessage("Rec: ");
      this->debugMessage(String(incomingByte));
      switch (*msgState) {
        case WAITING_FOR_STX:
          message = "";
          if (incomingByte != STX) { // first byte must be an STX
            transmitByte(NAK);
            this->debugMessage("Tranmitting NAK no STX");
            this->NAKcount++;
            return;
          }
          *msgState = WAITING_FOR_ETX;
          this->debugMessage("Waiting for ETX");
          break;
        case WAITING_FOR_ETX:
          if (incomingByte != ETX) {
            message += (char)incomingByte;
          } else { // ETX received
            uint16_t msgLen = message.length();
            uint8_t checksum = message[msgLen - 1];
            byte calculatedChecksum = calculateChecksum(message.substring(0, msgLen - 1));
            if (checksum == calculatedChecksum) { // successfully received message
              this->debugMessage("Send ACK");
              transmitByte(ACK);
              message = message.substring(0, msgLen - 1);
              *msgState = MESSAGE_RECEIVED;
            } else {  // Send NAK for checksum mismatch 
              transmitByte(NAK);
              this->debugMessage("Send NAK");
              NAKcount++;
              if (this->NAKcount == MAX_RETRIES + 1) 
                *msgState = ERROR;
              else
                *msgState = WAITING_FOR_STX;
            }
          }   
      } //switch
    } else { // no character available so lets check if we timeout
      unsigned long currentMillis;
      currentMillis = millis();
      if (currentMillis - this->timer >= TIMEOUT_MS)
        *msgState = TIMEOUT;    
    }
  }
  
