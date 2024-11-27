/*
  Terry Field, Student # 3721738
  COMP 444 
  November 18th, 2024

  This sketch implements the coding challenges for Project 1B of the SparkFun Inventor's Kit
  guide.

  The first part of the challenge is to introduce a scaling factor to the analog input and the second
  part of the challenge is to add additional LEDs that blink at different rates.

*/

int potPosition;       // this variable will hold a value based on the position of the potentiometer
int loop_count;        // this variable holds the loop count
const int unit_led  = 13; // This LED turns on and off for every loop in the loop() function
const int led_2_pin = 12;
const int led_3_pin = 11;

const float scale_factor = 2.0;

bool led_2_on = false;
bool led_3_on = false;

void setup()
{
  Serial.begin(9600);       //start a serial connection with the computer

  pinMode(unit_led, OUTPUT);      //set pin 13 as an output that can be set to HIGH or LOW
  pinMode(led_2_pin, OUTPUT);
  pinMode(led_3_pin, OUTPUT);
}

void loop()
{
  //read the position of the pot
  potPosition = analogRead(A0) * scale_factor;  //set potPosition to a number between (0 and 1023)*scale_factor based on how far the knob is turned
  Serial.println(potPosition); //print the value of potPosition in the serial monitor on the computer

  loop_count++;
  digitalWrite(unit_led, HIGH); // Turn on the LED

  // LED 2 blinks at half the frequency of the "unit LED"
  if (loop_count % 2 == 0) {
    if (led_2_on) {
      digitalWrite(led_2_pin, LOW);
    } else {
      digitalWrite(led_2_pin, HIGH);
    }
    led_2_on = !led_2_on;
  }
  
  // LED 3 blinks at a quarter of the frequency of the "unit LED"
  if (loop_count % 4 == 0) {
    if (led_3_on) {
      digitalWrite(led_3_pin, LOW);
    } else {
      digitalWrite(led_3_pin, HIGH);
    }
    led_3_on = !led_3_on;
  }  

  delay(potPosition); // delay for as many milliseconds as potPosition (0 to scale_factor*1023)

  digitalWrite(unit_led, LOW); // Turn off the LED
  delay(potPosition);          // delay for as many milliseconds as potPosition (0 to scale_factor*1023)
}


