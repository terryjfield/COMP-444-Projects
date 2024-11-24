/*
  Terry Field, Student # 3721738
  COMP 444 
  November 18th, 2024

  This sketch "transmits" a message in Morse code via an LED. Each letter to be transmitted is
  also output to the serial port for debugging.
*/

const int LED_PIN = 12; // Digital pin to which LED is connected to

/* 
  See https://morsecode.world/international/morse2.html for a description of
  International Morse Code.
*/
const int word_space_delay = 7;
const int dot_delay = 1;
const int inter_symbol_delay = 1;
const int dash_delay = 3;
const int inter_letter_delay = 3;
const int speed_factor = 250; // Adjust the speed factor to alter the words per minute

const int num_char_rows = 26;     // Number of letters in the alphabet
const int num_symbol_columns = 5; // Maximum number of symbols per Morse code character

/* 
  This array contains the definitions of the Morse code characters. The '!' symbol
  is used to denote the end of the character. 
  */
char morse_codes[num_char_rows][num_symbol_columns] = { 
  {'.','-','!'},         // A
  {'-','.','.','.','!'}, // B
  {'-','.','-','.','!'}, // C
  {'-','.','.','!'},     // D
  {'.','!'},             // E
  {'.','.','-','.','!'}, // F
  {'-','-','.','!'},     // G
  {'.','.','.','.','!'}, // H
  {'.','.','!'},         // I
  {'.','-','-','-','!'}, // J
  {'-','.','-','!'},     // K
  {'.','-','.','.','!'}, // L
  {'-','-','!'},         // M
  {'-','.','!'},         // N
  {'-','-','-','!'},     // O
  {'.','-','-','.','!'}, // P
  {'-','-','.','-','!'}, // Q
  {'.','-','.','!'},     // R
  {'.','.','.','!'},     // S
  {'-','!'},             // T
  {'.','.','-','!'},     // U
  {'.','.','.','-','!'}, // V
  {'.','-','-','!'},     // W
  {'-','.','.','-','!'}, // X
  {'-','.','-','-','!'}, // Y
  {'-','-','.','.','!'}  // Z
};

void transmit_letter(char letter) {
  if (letter == ' ') {
    Serial.print(' ');
    delay(word_space_delay * speed_factor);
    return;
  }
  int row = (int)letter - (int)'A';
  Serial.print(letter);
  for (int i = 0; i < num_symbol_columns; i++) {
    char symbol = morse_codes[row][i];
    int symbol_delay;
    switch (symbol) {
      case '.':
        symbol_delay = dot_delay;
        break;
      case '-':
        symbol_delay = dash_delay;
        break;
      case '!':
        return;
        break;
    }
    digitalWrite(LED_PIN, HIGH);
    delay(symbol_delay * speed_factor);
    digitalWrite(LED_PIN, LOW);
    delay(inter_symbol_delay * speed_factor);
  }
  delay((inter_letter_delay - inter_symbol_delay) * speed_factor); // already delayed by inter_symbol_delay
}

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

// Transmit the message in an endless loop
void loop() {
  String transmit_str = String("The quick brown fox jumps over the lazy dog "); // Message to transmit, only alpha chars
  transmit_str.toUpperCase();
  int str_len = transmit_str.length();
  for (int i = 0; i < str_len; i++) { // loop to iterate through the string 
    transmit_letter(transmit_str[i]);
  }
  Serial.println();
}
