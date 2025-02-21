/*
  Terry Field, Student # 3721738
  COMP 444 
  February 17th, 2025

  This sketch implements the coding challenges for Project 1D of the SparkFun Inventor's Kit
  guide.

  New behaviour includes pulsing the solid colours and cycling through all colours when the photoresistor
  value is above the threshold.

*/

int photoresistor;       // variable for storing photoresistor reading
int potentiometer;       // this variable stores a value based on the position of the knob
int threshold = 600;     // the threshold value below which to turn on the light, higher means more light on the photoresistor
float intensity = 0.0;   // intensity to display RGB values, 0% to 100%
bool increasing_brightness = true; // whether intensity is increasing or decreasing when pulsing the LED
unsigned int loop_count = 0;      // incremented every interation of the main loop

// LED pins
int red_pin = 9;
int green_pin = 10;
int blue_pin  = 11;

void setup() {
  Serial.begin(9600);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
}

void loop() {
  loop_count++; // used when cycling through colours

  photoresistor = analogRead(A0);
  potentiometer = analogRead(A1);

  Serial.print("Photoresistor value:");
  Serial.print(photoresistor);
  Serial.print("  Potentiometer value:");
  Serial.println(potentiometer);

  if (increasing_brightness) {
    intensity += 0.1;
  } else {
    intensity -= 0.1;
  }

  // Have we exceeded maximum brightness or exceeded maximum "darkness"? If so, reverse direction of change
  if (intensity > 1.0 || intensity < 0.0) {
    increasing_brightness = !increasing_brightness;
    if (intensity > 1.0)
      intensity = 1.0;
    else
      intensity = 0.0;    
  }

  if (photoresistor < threshold) { // if dark turn on the nightlight
    if (potentiometer >= 0 && potentiometer <= 150)
      red();
    if (potentiometer > 150 && potentiometer <= 300)
      orange();
    if (potentiometer > 300 && potentiometer <= 450)
      yellow();
    if (potentiometer > 450 && potentiometer <= 600)
      green();
    if (potentiometer > 600 && potentiometer <= 750)
      cyan();
    if (potentiometer > 750 & potentiometer <= 900)
      blue();
    if (potentiometer > 900)
      magenta();            
  } else {  // if it is not dark, cycle through all possible colours
    cycle_colours(loop_count);
  }
  delay(50); // short delay to make the serial output easier to read
}

/*
  This function calls the analogWrite functions to set the colours and intensity 
  of the RGB LED.
*/
void light_rgb(float red, float green, float blue, float intensity) {
  // Negative values of rgb are possible due to rounding of float operations
  if (red < 0.0)
    red = 0;
  if (green < 0.0) 
    green = 0;  
  if (blue < 0.0)
    blue = 0;
  // intensity range is from 0.0 to 1.0 and second parameter of analogWrite has a range of 0 to 255.     
  analogWrite(red_pin, int(intensity * 255 * red));
  analogWrite(green_pin, int(intensity * 255 * green));
  analogWrite(blue_pin, int(intensity * 255 * blue));  
}

void red() {
  light_rgb(1.0, 0.0, 0.0, intensity); // 100% red
}

void orange() {
  light_rgb(1.0, 0.5, 0.0, intensity); // 100% red, 50% green
}

void yellow() {
  light_rgb(1.0, 1.0, 0.0, intensity);
}

void green() {
  light_rgb(0.0, 1.0, 0.0, intensity);
}

void cyan() {
  light_rgb(0.0, 1.0, 1.0, intensity);
}

void blue() {
  light_rgb(0.0, 0.0, 1.0, intensity);
}

void magenta() {
  light_rgb(1.0, 0.0, 1.0, intensity);
}

/*
  This function steps through all possible 1000 combinations of RGB values
  in 10% intensity increments.

  Range of integers from 0 to 999, where 1's is the red value percentage, 10's the green value percentage, 
  100's the blue value percentage.
*/
void cycle_colours(int loop_count) {
  intensity = 0.50;
  light_rgb(loop_count % 10 / 10.0, 
    (loop_count % 100 - (loop_count % 10))/ 100.0, 
    (loop_count % 1000 - loop_count % 100 - loop_count % 10)/ 1000.0, intensity);
}
