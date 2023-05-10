#include <SoftwareSerial.h>
#include <Wire.h>
// All the necessary pins and other values are created
const int sound = A0;
const int redPin = 10;
const int greenPin = 12;
const int threshold_perc = 10;
const int timeBetweenCheck = 500;
int i = 1;

unsigned long sCount = 0;
unsigned long msStart = millis();
unsigned long last_event = 0;
unsigned int y = 1;

#include <VarSpeedServo.h> 
#include "SR04.h"
#define TRIG_PIN 9
#define ECHO_PIN 8
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
VarSpeedServo myservo;  // create servo object to control a servo
long dis;
int triggerCount = 0;   // Initialize the counter for the number of times the SR04 has been triggered





void setup() {
  // We make sure that the pins are set up properly and start the program in the default state with the red light on
  pinMode(sound, INPUT); 
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(redPin,HIGH);
    myservo.attach(7);
}

void loop() {
  int actual_perc = (float) analogRead (sound) / (float) 1023 * 100; //make the value read from the soundsensor into a percentage
  Serial.print("Actual perc: ");
  Serial.println(actual_perc);
  i = i+1;
  delay(timeBetweenCheck);
  sCount = y*timeBetweenCheck/100; // value for the time that the hands are being washed in seconds
  if (actual_perc >= threshold_perc) {  // check if the sound measured is higher than the threshold and if so, turn on green light and turn off red
    if (millis() - last_event > 250) {
      digitalWrite(redPin,LOW);
      digitalWrite(greenPin,HIGH);
    }
    last_event = millis();
  }
 if(i%11==0){ // a modulo check so that every 10 times we go through the loop (every 5 sec due to 500 delay), it checks the below code
   if(actual_perc < threshold_perc){ // if the value is below the threshold when checked we assume the washing has ended
      digitalWrite(greenPin,LOW);
      digitalWrite(redPin,HIGH); 
      Serial.print("You washed your hands for ");
      Serial.print(sCount
      );
      Serial.println(" seconds");
      if(sCount >= 20){ // check if the sound was consistantly over the threshold for 4 or more cycles (>=20 sec) 
        Serial.println("HANDS ARE CLEAN!!!");
        digitalWrite(redPin,LOW);
        for(int win = 0; win < 5; win++){ //small animation to indicate the washing of hands was good
          digitalWrite(greenPin,HIGH);
          delay(200);
          digitalWrite(greenPin,LOW);
          delay(200);
        }
        digitalWrite(redPin,HIGH);
      }
      else{
        Serial.println("You need to wash hands for atleast 20 sec");
        for(int win = 0; win < 5; win++){ // small animation if the hand washing wasn't good enough
          digitalWrite(redPin,HIGH);
          delay(200);
          digitalWrite(redPin,LOW);
          delay(200);
        }
        digitalWrite(redPin,HIGH);
      }
      y=1; // resets timer
   }
   else if (actual_perc >= threshold_perc){ // Counts amount of times the washing is going on (checks every 5 sec)
     y=y+1;
   }
   i=1; // resets modulo timer
  }

  // Function from the SR04.h libary that takes the input from the SR04 sensor, and outputs the distance
  dis = sr04.Distance();
  // Simple if statement that takes in the messured distance, and asks if the distance of over a certian threshold, if it is the servo will open, and if not the servo will close
  if(dis < 25){
    Serial.print('\n');
    Serial.print(dis);
    Serial.println("cm");
    myservo.write(180, 30, true);
  }else {
    Serial.print('\n');
    Serial.print(dis);
    Serial.println("cm");
    myservo.write(0, 30, true);
  
  }






}