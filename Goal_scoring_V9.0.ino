// Libraries
#include <Wire.h>
#include "rgb_lcd.h"
#include "Ultrasonic.h"
#include "Arduino.h"
 
// Pin definitions
const int speakerPin = 8;                                   // the number of the speaker pin, D8
 
// Output initialization
rgb_lcd lcd;                                                // LCD display
Ultrasonic ultrasonic(7);                                   // Ultrasonic sensor connected to pin D7
 
// Variables
int Detect1;                                                // Detection variable for the ultrasonic sensor
static int Score1 = 0;                                      // Score variable for the sensor
int colorR = 0;                                             // Red color
int colorG = 0;                                             // Green color
int colorB = 0;                                             // Blue color
unsigned long lastDetectTime = 0;                           // Last time a detection was made
unsigned long startTime = 0;                                // Start time of the game
bool doublePointMode = false;                               // Flag for double point mode
unsigned long doublePointModeStartTime = 0;                 // Start time of double point mode
 
// Define the notes and their frequencies
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
 
// Define the duration of each note (in milliseconds)
#define EIGHTH_NOTE 250
#define QUARTER_NOTE 500
 
// Define the victory noise melody and note durations
int victoryMelody[] = {
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5
};
 
int victoryDurations[] = {
  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE
};
 
void setup() {
  // Set pin mode
  pinMode(speakerPin, OUTPUT);                              // Speaker pin as output
 
  // Initialize LCD and start serial communication
  lcd.begin(16, 2);                                         // Initialize LCD
  Serial.begin(9600);                                       // Start serial communication
 
  NewGame();                                                //trigger the newGame() function
}
 
void loop() {
  // Read distance from ultrasonic sensor
  Detect1 = ultrasonic.MeasureInCentimeters();              // Detect distance in centimeters
 
  // Logic Goal scoring
  unsigned long currentTime = millis();                     
  if (Detect1 < 5 ) {                                       // If the distance is less than 5 cm
    ScoreUpdate();                                          //ScoreUpdate function is triggered
    lastDetectTime = currentTime;                           //Variable LastDetectTime is updated to currentTime
  }
 
  Displaycolors();
 
  // Check if 3 minutes have passed
  if (currentTime - startTime >= 180000 && !doublePointMode) {
    doublePointMode = true;
    doublePointModeStartTime = currentTime;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Double Point Mode!");
    delay(2000);
  }
 
  // If player wins
  if (Score1 == 10) {
    Score1 = 0;                                // Reset the score to 0
    Serial.println("Sensor 1 - You Won!");     // Print victory message to the serial monitor
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Congrats!");                   // Display "Congrats!"
    lcd.setCursor(0, 1);
    lcd.print("You won");                     // Display "You won" message
    // Play the victory noise
    playVictoryNoise();
    delay(2000);                               // Delay for 2 seconds before restarting game
    // Start a new game
    NewGame();
    
  }
 
  delay(200);                                   // Delay to avoid multiple detections
}
 
// Function to play the victory noise
void playVictoryNoise() {
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    // Calculate the duration of each note
    int noteDuration = victoryDurations[thisNote];
    tone(speakerPin, victoryMelody[thisNote], noteDuration);
 
    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // Stop the tone
    noTone(speakerPin);
  }
}
 
// Display starting message on LCD
void NewGame() {
  lcd.clear();
  lcd.setCursor(0, 0);                // Set cursor at the first position
  lcd.print("New Game");              // Display "New Game"
  lcd.setCursor(0, 1);                // Set cursor at the second line
  lcd.print("Score: 0");              // Display "Score: 0"
 
  Score1 == 0;
  startTime = millis();               // Record the start time of the game
  doublePointMode = false;            // Reset double point mode flag
}
 
void ScoreUpdate() {
  if (doublePointMode) {
    Score1 += 2;                      // Double points
  } else {
    Score1++;                         // Normal points
  }
  Serial.print("Sensor 1 - Goal Scored! Score: ");
  Serial.println(Score1);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: "); lcd.print(Score1); // Update score on LCD
  lcd.setCursor(0, 1);
  lcd.print(10 - Score1); lcd.print(" more to go"); // Display how many more to go
}
 
void Displaycolors() {
  lcd.setRGB(colorR, colorG, colorB);

  // Blue
  if (Score1 > 3 && Score1 < 9) {
    colorB = 255;
  } else {
    colorB = 0;
  }
 
  // Green
  if (Score1 > 0 && Score1 < 5) {
    colorG = 255;
  } else if (Score1 == 5) {
    colorG = 125;
  }  else if (Score1 == 0) {
    colorG = 102;
  } else {
    colorG = 0;
  }
 
  // Red
  if (Score1 == 1 || Score1 == 0) {
    colorR = 255;
  } else if (Score1 == 2 || Score1 == 7) {
    colorR = 125;
  } else if (Score1 > 7 && Score1 < 10) {
    colorR = 255;
  } else {
    colorR = 0;
  }
}