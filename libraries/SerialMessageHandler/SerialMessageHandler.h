/*
  SerialMessageHandler.h - Library for sending and receiving messages over a serial port.
  Created by Terry Field 3rd May 2025
*/
#ifndef SerialMessageHandler_h
#define SerialMessageHandler_h

#include "Arduino.h"
#include "SoftwareSerial.h"

const byte STX = 0x02;
const byte ETX = 0x03;
const byte ACK = 0x06;
const byte NAK = 0x15;

enum protoState { READY,    // The serial port is ready to transmit a new message
                  TIMEOUT,  // No response was received 
                  ERROR,    // Message not successfully received or transmitted
                  WAITING_FOR_ACK, // Message transmitted, waiting for receiver to send ACK
                  ACK_RECEIVED,    // Receiver sent ACK
                  NAK_RECEIVED,    // Receiver sent NAK
                  WAITING_FOR_STX, // Waiting for start of message transmission
                  WAITING_FOR_ETX, // Received STX, receiving bytes until STX received
                  MESSAGE_RECEIVED // Message successfully received
                  };

const byte MAX_RETRIES = 3; // Number of times to retry transmission of message if receiver NAKs it
const unsigned long TIMEOUT_MS = 3000; // Time to wait for receiving message

class SerialMessageHandler
{
  public:
    SerialMessageHandler(int rxPin, int txPin, HardwareSerial& serialPort, bool waitForAck); //constructor
    void debugMessage(String message);
    void sendMessage(protoState *msgState, String message);
    void receiveMessage(protoState *msgState, String &message);
    void listen();

  private:
    SoftwareSerial *softSerial;
    HardwareSerial *hw_serial;
    uint32_t   timer;
    uint8_t    retransmitCount;        // stores of times the message has been re-transmitted
    uint8_t    NAKcount;               // stores the number of times a received message has been NAK'd
    protoState msgState = READY;       // Initial state
    bool       waitForAck;             // Flag to indicate whether to wait for an ack or not

    byte calculateChecksum(String message);
    void transmitFrame(String message);
    protoState receiveACK();
    protoState transmitMessage(String message);
    void transmitByte(byte value);
};

#endif