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
