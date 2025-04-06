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

// Pins for RGB LED
const int RED_PIN = 6;
const int BLUE_PIN = 5;

// make some custom characters to represent the bar chart:
byte bar1[8] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
};

byte bar2[8] = {
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
};

byte bar3[8] = {
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
};

byte bar4[8] = {
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
};

byte bar5[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
};


void setup() {
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin (9600);        //set up a serial connection with the computer

  lcd.begin(16, 2); // initialize a 16x2 display
  lcd.clear();       // clear the display
  lcd.createChar(0, bar1);
  lcd.createChar(1, bar2);
  lcd.createChar(2, bar3);
  lcd.createChar(3, bar4);
  lcd.createChar(4, bar5);
};

const int ROW_LEN = 16;
int intDegreesC;
char rowBuffer[ROW_LEN + 1]; // add 1 for null char
float blueIntensity; // percentage of blue colour in the RGB LED 

void loop() {
  
  voltage = analogRead(A0) * 0.004882813;   //convert the analog reading, which varies from 0 to 1023, back to a voltage value from 0-5 volts
  degreesC = (voltage - 0.5) * 100.0;       //convert the voltage to a temperature in degrees Celsius
  Serial.print("Temperature: ");
  Serial.println(degreesC);
  lcd.clear();
  
  intDegreesC = degreesC * 100; // int containing the temp in 100th's of degrees
  sprintf(rowBuffer, "%02u       %02u", intDegreesC / 100, intDegreesC / 100 + 1);
  lcd.setCursor(0,0);
  lcd.print(rowBuffer);

  lcd.setCursor(0,1);
  int tenthsUnits = intDegreesC % 100 / 10;
  for (int i = 0; i < tenthsUnits; i++) {
    lcd.write((byte)4);
  }

  int hundredthsUnits = intDegreesC % 10;
  if (hundredthsUnits == 0 && tenthsUnits == 0) // display the thinnest bar to indicate that there is a reading
    lcd.write((byte)0);
  else if (hundredthsUnits == 9) { // display full bar
    lcd.write((byte)4);
  }  
  else {
      lcd.write((byte)(hundredthsUnits / 2 - 1)); 
  }

  // The tenths of a degree controls the mix of blue and red light. The greater the tenths units, the more red (hotter)
  if (tenthsUnits <= 2) // pure blue for 2 or less
    tenthsUnits = 0;
  else if (tenthsUnits >= 8) // pure read for 8 or more
    tenthsUnits = 10;
  blueIntensity = (10 - tenthsUnits)/10.0;
  analogWrite(BLUE_PIN, 128*blueIntensity);
  analogWrite(RED_PIN, 128*(1.0 - blueIntensity));
  Serial.print("Blue: ");
  Serial.print(128*blueIntensity);
  Serial.print(" Red: ");
  Serial.print(128*(1.0 - blueIntensity));
  Serial.print(" Tenths units: ");
  Serial.println(tenthsUnits);
  delay(1000);
}
