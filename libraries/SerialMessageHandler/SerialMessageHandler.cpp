#include "SerialMessageHandler.h"

SerialMessageHandler::SerialMessageHandler(int rxPin, int txPin, HardwareSerial &serialPort, bool waitForAck) {
    //this->softSerial = new SoftwareSerial(rxPin, txPin);
    this->softSerial = new SoftwareSerial(rxPin, txPin);
    this->softSerial->begin(9600);
    this->hw_serial  = &serialPort;
    this->waitForAck = waitForAck;
}

void SerialMessageHandler::listen() {
  this->softSerial->listen();
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
  
  void SerialMessageHandler::transmitFrame(String message) {
    String frame;
    byte frameLen;
  
    byte chksum = calculateChecksum(message);
    frame = (char)STX;
    frame += message;
    frame += (char)chksum;
    frame += (char)ETX;
    frameLen = frame.length();
    for (int i = 0; i < frameLen; i++) {
      this->softSerial->write(frame[i]);  
    }
    this->softSerial->flush();
    this->timer = millis(); // capture the time when the message was sent
  }
  
  /**
    Wait for an ACK character. 
  */
  protoState SerialMessageHandler::receiveACK() {
    byte incomingByte = 0;
    if (this->softSerial->available() > 0) {
      incomingByte = this->softSerial->read();
      switch (incomingByte) {
        case ACK:
          return ACK_RECEIVED;
          break;
        case NAK:
          return NAK_RECEIVED;
          break;
        default:
          this->softSerial->write(NAK);
          this->softSerial->flush();
          return ERROR;
      };
    } else {
      unsigned long currentMillis;
      currentMillis = millis();
      if (currentMillis - this->timer >= TIMEOUT_MS) {  // Timeout
        return TIMEOUT;
      }
    }
    return WAITING_FOR_ACK;
  }
  
  protoState SerialMessageHandler::transmitMessage(String message) {
    transmitFrame(message);
    return WAITING_FOR_ACK;
  }
  
  void SerialMessageHandler::sendMessage(protoState *msgState, String message) {
    protoState localState;
    localState = *msgState;
    switch (localState) {
      case READY:
        transmitMessage(message);
        if (this->waitForAck)
          *msgState = WAITING_FOR_ACK;
        else
          *msgState = ACK_RECEIVED;  
        this->retransmitCount = 0;
        break;
      case WAITING_FOR_ACK:
        *msgState = receiveACK();
        break;
      case NAK_RECEIVED:
        if (this->retransmitCount == MAX_RETRIES) {
          *msgState = ERROR;
          return;
        } else {
          transmitMessage(message);
          *msgState = WAITING_FOR_ACK;
          this->retransmitCount++;
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
      this->timer = millis();  // capture the current time. We will refresh this time if we receive an STX
      this->debugMessage("Waiting for STX");
    }
    if (this->softSerial->available() > 0) {  // Check if character is available, non-blocking
      incomingByte = this->softSerial->read();
      switch (*msgState) {
        case WAITING_FOR_STX:
          message = "";
          if (incomingByte != STX) { // first byte must be an STX
            transmitByte(NAK);
            this->debugMessage("Tranmitting NAK no STX");
            this->NAKcount++;
          } else {
            *msgState = WAITING_FOR_ETX;
            this->timer = millis(); // Now reset the timeout timer now that we have received the STX
            this->debugMessage("Waiting for ETX");
          }
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
              NAKcount++;
              this->timer = millis(); // reset timer for sender to re-transmit
              this->debugMessage("Send NAK");
              if (this->NAKcount == (MAX_RETRIES + 1)) 
                *msgState = ERROR;
              else
                *msgState = WAITING_FOR_STX;
            }
          }   
      } //switch
    } else { // no character available so let's check if we timeout
      unsigned long currentMillis;
      currentMillis = millis();
      if ((currentMillis - this->timer) >= TIMEOUT_MS)
        *msgState = TIMEOUT;    
    }
  }
  
