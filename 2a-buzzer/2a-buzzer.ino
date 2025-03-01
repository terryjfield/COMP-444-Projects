/*
  Terry Field, Student # 3721738
  COMP 444 
  February 17th, 2025

  This sketch implements the coding challenges for Project 2A of the SparkFun Inventor's Kit
  guide.

*/

int speakerPin = 10;  // pin that the buzzer is connected to

void setup() {
  pinMode(speakerPin, OUTPUT);
}

// "Moon River" is a song composed in 1961 by Henry Mancini with lyrics by Johnny Mercer. 
void loop() {
  play('g', 6);
  play('D', 2);
  play('C', 4);
  play('b', 3);
  play('a', 1);
  play('g', 1);
  play('f', 1);
  play('g', 4);
  play('c', 1);
  play('b', 3);
  play('a', 1);
  play('g', 1);
  play('f', 1);
  play('g', 4);
  play('c', 2);
  play('d', 6);
  play('d', 4);
  play('e', 2);
  play('c', 6);
  play('g', 2);
  play('e', 3);
  play('d', 1);
  play('c', 6);
  play('g', 2);
  play('e', 3);
  play('d', 1);
  play('c', 2);
  play('e', 2);
  play('g', 2);
  play('C', 2);
  play('b', 3);
  play('a', 1);
  play('b', 2);
  play('a', 3);
  play('g', 1);
  play('a', 6);

  while (true) {}
}

void play(char note, int beats) {
  int numNotes = 14;

 //Note: these notes are C major (there are no sharps or flats)

  //this array is used to look up the notes
  char notes[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B', ' '};
  //this array matches frequencies with each letter (e.g. the 4th note is 'f', the 4th frequency is 175)
  int frequencies[] = {131, 147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 0};

  int currentFrequency = 0;    //the frequency that we find when we look up a frequency in the arrays
  int beatLength = 200;   //the length of one beat (changing this will speed up or slow down the tempo of the song)

  for (int i = 0; i < numNotes; i++) {
    if (notes[i] == note) {
      currentFrequency = frequencies[i];
    }
  }

  tone(speakerPin, currentFrequency, beats * beatLength);
  delay(beats * beatLength);
  delay(50);
}
