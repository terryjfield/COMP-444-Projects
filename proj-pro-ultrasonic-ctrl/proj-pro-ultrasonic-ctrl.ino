/*
  Terry Field, Student # 3721738
  COMP 444 
  April 12th, 2025


*/

#include <NewPing.h>
#include "DHT.h"
#include <SoftwareSerial.h>  

// Define pins for the 4 HC-SR04 ultrasonic sensors
const byte FRONT_PIN = 2;          
const byte REAR_PIN  = 3;
const byte RIGHT_PIN = 4;
const byte LEFT_PIN  = 5;          

// Initialize DHT sensor
const byte DHT_PIN = 10;
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// Instantiate NewPing objects for the 4 ultrasonic sensors
NewPing frontSonar(FRONT_PIN, FRONT_PIN, 300); 
NewPing rearSonar(REAR_PIN, REAR_PIN, 300);
NewPing rightSonar(RIGHT_PIN, RIGHT_PIN, 300);
NewPing leftSonar(LEFT_PIN, LEFT_PIN, 300);

float temp = 20.5; // Temperature in Celsius (this value updated from the DHT22 sensor)
float humidity = 0.50; 


// BlueTooth
const byte bluetoothTx = 6;  // TX-O pin of bluetooth mate, Arduino D6
const byte bluetoothRx = 7;  // RX-I pin of bluetooth mate, Arduino D7

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
  bluetoothSetup();
  dht.begin();
}

void loop() {
  delay(100);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  bluetooth.print("Ping front: ");
  bluetooth.println(getDistance(frontSonar));
  bluetooth.print("Ping rear: ");
  bluetooth.println(getDistance(rearSonar));
  bluetooth.print("Ping right: ");
  bluetooth.println(getDistance(rightSonar));
  bluetooth.print("Ping left: ");
  bluetooth.println(getDistance(leftSonar));
  getTempAndHumidity(&temp, &humidity);
  bluetooth.print("Temperature: ");
  bluetooth.println(temp);
  bluetooth.print("Humidity: ");
  bluetooth.println(humidity);
  serialPrint("Foo");
  delay(2000);
}

template<typename T>
void serialPrint(T value) {
  Serial.print(value);
}

template<typename T>
void serialPrintln(T value) {
  Serial.println(value);
}

void getTempAndHumidity(float *temp, float *humidity) {
  *humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  *temp = dht.readTemperature();  
}

// Returns the distance measured by the HC-SR04 distance sensor
float getDistance(NewPing &sonar)
{
  float factor = sqrt(1 + temp / 273.15) / 60.368; // Speed of sound calculation based on temperature.
  return (float)sonar.ping_median(5) * factor;     //send back the distance that was calculated
}
