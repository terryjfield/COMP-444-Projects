/*
  Terry Field, Student # 3721738
  COMP 444 
  March 25th, 2025

  This sketch implements the coding challenges for Project 4b of the SparkFun Inventor's Kit
  guide. 

*/

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);    // tell the RedBoard what pins are connected to the display

float voltage  = 0;   // voltage measured from the TMP36
float degreesC = 0;   // temperature calculated in degrees Celsius
float degreesF = 0;   // temperature calculated in degress Fahrenheit

void setup() {
  
  lcd.begin(16, 2); // initialize a 16x2 display
  lcd.clear();       // clear the display
}

void loop() {
  
  voltage = analogRead(A0) * 0.004882813;   //convert the analog reading, which varies from 0 to 1023, back to a voltage value from 0-5 volts
  degreesC = (voltage - 0.5) * 100.0;         //convert the voltage to a temperature in degrees Celsius
  degreesF = degreesC * (9.0 / 5.0) + 32.0;   //convert Celsius to Fahrenheit

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Degrees F: ");
  lcd.print(degreesF);

  lcd.setCursor(0,1);
  lcd.print("Degrees C: ");
  lcd.print(degreesC);

  delay(1000);

}
