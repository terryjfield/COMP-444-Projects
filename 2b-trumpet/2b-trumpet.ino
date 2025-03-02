/*
  Terry Field, Student # 3721738
  COMP 444 
  March 1st, 2025

  This sketch implements the coding challenges for Project 2B of the SparkFun Inventor's Kit
  guide. 

*/

//set the pins for the button and buzzer
int firstKeyPin = 2;
int secondKeyPin = 3;
int thirdKeyPin = 4;

int buzzerPin = 10;
int keyValue = 0; // value computed from pressed keys

void setup() {
  //set the button pins as inputs
  pinMode(firstKeyPin, INPUT_PULLUP);
  pinMode(secondKeyPin, INPUT_PULLUP);
  pinMode(thirdKeyPin, INPUT_PULLUP);

  //set the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  // Each button will set a bit in the keyValue variable, which can range from 0 to 7.
  keyValue = 0;
  if (digitalRead(firstKeyPin) == LOW) {      
    keyValue = 1;                        
  }
  
  if (digitalRead(secondKeyPin) == LOW) { 
    keyValue = keyValue | 2;  // Set 2nd bit     
  }
  
  if (digitalRead(thirdKeyPin) == LOW) { 
    keyValue = keyValue | 4; // Set 3rd bit               
  }
  
  switch (keyValue) { // Using a switch/case statements simpler than using if statements.
    case 0: noTone(buzzerPin);  //if no key is pressed turn the buzzer off
        break;
    case 1: tone(buzzerPin, 262);
        break;
    case 2: tone(buzzerPin, 294);
        break;
    case 3: tone(buzzerPin, 330);
        break;
    case 4: tone(buzzerPin, 392);
        break;
    case 5: tone(buzzerPin, 440);
        break;
    case 6: tone(buzzerPin, 494);
        break;
    case 7: tone(buzzerPin, 523);                           
  }
}

/*
  note  frequency
  c     262 Hz
  d     294 Hz
  e     330 Hz
  f     349 Hz
  g     392 Hz
  a     440 Hz
  b     494 Hz
  C     523 Hz
*/
