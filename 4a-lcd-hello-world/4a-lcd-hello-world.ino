/*
  Terry Field, Student # 3721738
  COMP 444 
  March 18th, 2025

  This sketch implements the coding challenges for Project 4A of the SparkFun Inventor's Kit
  guide. 

*/

#include <LiquidCrystal.h>    // the liquid crystal display library

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

const int buttonPin = 2;
bool messageOne = true;

void setup() {
  
  lcd.begin(16,2); // we are using a 16x2 display
  lcd.clear();

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  lcd.setCursor(0, 0);

  if (digitalRead(buttonPin) == LOW) {      
    messageOne = !messageOne;                        
  }
  if (messageOne)
    lcd.print("May the Force");
  else
    lcd.print("be with you! ");  

  lcd.setCursor(0, 1);
  int totalSecs  = millis() / 1000;
  char buffer[16];
  sprintf(buffer, "%02u:%02u:%02u", totalSecs / 3600, totalSecs / 60 % 60, totalSecs % 60);
  lcd.print(buffer);
  delay(100);
}
