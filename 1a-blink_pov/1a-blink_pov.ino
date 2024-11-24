/*
  Terry Field, Student # 3721738
  COMP 444 
  November 18th, 2024

  This sketch when run turns on an LED for 2 seconds and then turns it off for 2 seconds and repeats.
  It also monitors the serial input and the user can enter a new delay period, in milliseconds, to override 
  the default 2000 ms.
*/

const int LED_PIN = 12;
int light_delay = 2000; // Initial value for delay period (ms)

/*
  This function is called once when the board is powered on/reset or when the 
  sketch first starts.
*/
void setup() {
  pinMode(LED_PIN, OUTPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Enter a new delay period in milliseconds and press ENTER:");
}

/*
  This is in the infinite loop that does the work. 
*/
void loop() {
  char input_char;
  String input_str;

  while (Serial.available()) {
    input_char = Serial.read();
    if (input_char == '\n') {
      light_delay = input_str.toInt();
      if (light_delay == 0) {
        light_delay = 2;
      }
      String message = "Delay set to " + String(light_delay);
      Serial.println(message);
    } else if (isDigit(input_char)) {
      input_str+= (char)input_char;
    }
  }

  digitalWrite(LED_PIN, HIGH); // set voltage level to HIGH, i.e. 5 volts
  delay(light_delay); // time is in millseconds, i.e. 1 second
  digitalWrite(LED_PIN, LOW); // set voltage level to LOW, i.e. to ground
  delay(light_delay);
}
