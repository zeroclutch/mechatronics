#include "timeouts.h"
# define CHANNEL_A 2
# define CHANNEL_B 3
# define LED_PIN 13


volatile long counter = 0;

const float countsPerRotation = 886.0;
const float circumference = 0.1885;
long distanceTravelled = 0;

volatile unsigned long lastChannelATime = 0;
volatile unsigned long lastChannelBTime = 0;
volatile unsigned long currChannelATime = 0;

bool hasPrinted = true;

double getDistance() {
  return (double) counter / countsPerRotation * circumference;
}

void setup() {
  // Configure pins
  pinMode(CHANNEL_A, INPUT);
  pinMode(CHANNEL_B, INPUT);

  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);

  // Configure ISRs
  attachInterrupt(digitalPinToInterrupt(CHANNEL_A), readChannelA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANNEL_B), readChannelB, CHANGE);

  // Toggle LED
  setInterval(&toggleLED, 500);
}

void readChannelA() {
 currChannelATime = millis();
 // Check whether B's pulse was closer to this pulse or the last one.
 // If it is closer to this pulse, we are moving from B to A
 // If it is closer to the last pulse, we aremillis() - lastChannelBTime moving from A to B
 long d2 = lastChannelBTime - lastChannelATime;
 long d1 = currChannelATime - lastChannelBTime;
 if(d1 > d2) {
   counter++;
 } else {
   counter--;
 }


 hasPrinted = false;
 lastChannelATime = currChannelATime;
}


void readChannelB() {
 lastChannelBTime = millis();
}

void blinkLED() {
  toggleLED();
  setTimeout(&toggleLED, 200);
}

void toggleLED() {
  int value = digitalRead(LED_PIN);
  digitalWrite(LED_PIN, !value);
}

void loop() {
  if(!hasPrinted) {
     Serial.println(getDistance());
  }
  readTimeouts();
}
