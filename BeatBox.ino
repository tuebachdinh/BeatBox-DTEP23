#include <EEPROM.h>

//DEFINE PIN
const int xPin = A2;
const int yPin = A1;
const int zPin = A0;
const int ledPin = 9;
const int buzzerPin = 6;
const int buttonPin = 3;


//Constant to calculate speed and save data
int eepromAddress1 = 0;
int eepromAddress2 = sizeof(float);
int eepromAddress3 = sizeof(float)*2;
int sensorOutput = 300;
float gravity = 9.81;


//DEFINE INDEX
int i = 0;
int k = 0;


//LAST VALUE RECORDER
int lastXValue = 0;
int lastYValue = 0;
int lastZValue = 0;


//VALUE
int timeBetweenPunch = 300;
int changeValue = 120;
float distance = 0;
int noteDuration = 250;
int numberOfSong = 10;


//FLAG
bool isBlinking = false;
bool punchDetected = false;
bool isPlugged = false;


//Data of user
int numberOfPunches = 0;
float maxVelocity = 0;
unsigned long unPluggedTime = 0;


//MELODY
int melodies[3][30] = {
  {392, 523, 523, 587, 523, 494, 440, 440, 440, 587, 587, 659, 587, 523, 494, 392, 392, 659, 659, 740, 659, 587, 523, 440, 392, 392, 440, 587, 494, 523},
  {262, 262, 392, 392, 440, 440, 392, 349, 349, 330, 330, 294, 294, 262, 392, 392, 349, 349, 330, 330, 294, 392, 392, 349, 349, 330, 330, 294, 262, 262},
  {392, 392, 440, 392, 523, 494, 392, 392, 440, 392, 587, 523, 392, 392, 784, 659, 523, 523, 494, 440, 698, 698, 659, 523, 587, 523, 392, 392, 440, 392}

};



float convertToVelocity(float distance){

  float velocity = (distance/sensorOutput)*gravity*timeBetweenPunch/1000;
  return velocity;

}


//SETUP
void setup() {

  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  isPlugged = true;
  int storedValue1;
  float storedValue2;
  unsigned long storedValue3;


  EEPROM.get(eepromAddress1, storedValue1);
  EEPROM.get(eepromAddress2, storedValue2);
  EEPROM.get(eepromAddress3, storedValue3);


    // Send the stored values to the computer
  Serial.print("\n\n");
  Serial.print("Number of punches: ");
  Serial.println(storedValue1);
  Serial.print("Maximum velocity: ");
  Serial.print(storedValue2);
  Serial.println(" m/s");
  Serial.print("Workout session: ");
  Serial.print(storedValue3/1000);
  Serial.println(" s");


}




//LOOP
void loop() {

  //READ AND CALCULATE VALUES
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int zValue = analogRead(zPin);
  distance = sqrt(pow(xValue - lastXValue, 2) + pow(yValue - lastYValue, 2) + pow(zValue - lastZValue, 2));




  //PRINT
  /*Serial.print("Distance changed: ");
  Serial.print(distance);
  /*Serial.print("\tPunch detected: ");
  Serial.print(punchDetected);
  Serial.print("\tIndex k: ");
  Serial.println(k);*/
    // Check if the Arduino is unplugged*/
  
  //CHANGE SONG
  if (digitalRead(buttonPin) == HIGH){
    k++;
    i = 0;
    numberOfPunches = 0;
    digitalWrite(ledPin, HIGH);
    maxVelocity = 0;
    unPluggedTime = 0;
    isPlugged = false;
    if (k >= 3){
      k = 0;
    }
  } else{
    digitalWrite(ledPin, LOW);
  }
  
  
  if (!isPlugged) {
    unPluggedTime = millis();
    EEPROM.put(eepromAddress3, unPluggedTime);
  }




  //PUNCH AND NOTE
  if (distance > changeValue && !isBlinking && !punchDetected) {
    tone(buzzerPin, melodies[k][i], noteDuration);
    digitalWrite(ledPin, HIGH);
    
    if (distance < 800 && maxVelocity < convertToVelocity(distance)){
      maxVelocity = convertToVelocity(distance);
    }
    numberOfPunches++;
    delay(100);
    noTone(buzzerPin);
    i = (i+1) % 30;
    punchDetected = true;
    EEPROM.put(eepromAddress1, numberOfPunches);
    EEPROM.put(eepromAddress2, maxVelocity);

  }
  
  else{
    punchDetected = false;
    digitalWrite(ledPin, LOW);
  }


  //UPDATE VALUE
  lastXValue = xValue;
  lastYValue = yValue;
  lastZValue = zValue;

  //DELAY
  delay(timeBetweenPunch);

}
