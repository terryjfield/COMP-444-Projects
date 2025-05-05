// Blue board
#include "SerialMessageHandler.h"                                                                                            "

protoState msgState = READY;

SerialMessageHandler smh(10,11,Serial);
boolean messageToSend = true;

void setup() {
  Serial.begin(9600);
  smh.debugMessage("hey!");
}

void loop() {
  if (messageToSend) {
    smh.sendMessage(&msgState, "S000,000");
    switch (msgState) {
      case ACK_RECEIVED:
        Serial.println("ACK received, message sent");
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
  delay(200);
}
