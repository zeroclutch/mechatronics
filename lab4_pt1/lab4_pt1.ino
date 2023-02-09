#include <assert.h>
#include <stdint.h>
#include "timeouts.h"

#define DEBUG_MODE 1

// Motor definitions
#define PIN_ENA 9
#define PIN_IN1 48
#define PIN_IN2 49
#define PIN_ENB 8
#define PIN_IN3 50
#define PIN_IN4 51
#define PIN_COUNT 6

// Distance definitions
#define CHANNEL_A 3
#define CHANNEL_B 4

#define SWITCH_COUNT 4

uint8_t switchPins[SWITCH_COUNT] = {24, 25, 26, 27};

enum STATES {
  FORWARD     = 0,
  REVERSE     = 1,
  BRAKE       = 2,
  COAST       = 3,
  TURN_LEFT   = 4,
  TURN_RIGHT  = 5,
  PIVOT_LEFT  = 6,
  PIVOT_RIGHT = 7,

  LENGTH = 8
};

int previousState = -1;
int currentState = BRAKE;

float currentSpeed = 0.1;
float previousSpeed = currentSpeed;

uint8_t pinStates[LENGTH][PIN_COUNT] = {
  //ENA,IN1,IN2,ENB,IN3,IN4
  {   1,  1,  0,  1,  1,  0}, // FORWARD
  {   1,  0,  1,  1,  0,  1}, // REVERSE
  {   1,  1,  1,  1,  1,  1}, // BRAKE
  {   0,  1,  0,  0,  1,  0}, // COAST
  {   1,  1,  0,  0,  1,  0}, // TURN_LEFT
  {   0,  1,  0,  1,  1,  0}, // TURN_RIGHT
  {   1,  1,  0,  1,  0,  1}, // PIVOT_LEFT
  {   1,  0,  1,  1,  1,  0}  // PIVOT_RIGHT
};

// Counter and distance 
volatile long counter = 0;

const float countsPerRotation = 886.0;
const float circumference = 0.1885;
volatile int totalDistance = 0;

volatile unsigned long lastChannelATime = 0;
volatile unsigned long lastChannelBTime = 0;
volatile unsigned long currChannelATime = 0;

/**
 * Sets the pins to the specified state
*/
void setPins(uint8_t enA, uint8_t in1, uint8_t in2, uint8_t enB, uint8_t in3, uint8_t in4, int state) {
  if(state != BRAKE) {
    assert(in1 != in2);
    assert(in3 != in4);
  }

  if(DEBUG_MODE) Serial.print("Setting pins. ");
  if(DEBUG_MODE) Serial.print("State = ");
  if(DEBUG_MODE) Serial.println(currentState);

  /** LEFT WHEEL **/

  // Turn off enable momentarily so we don't accidentally brake during switching
  if(DEBUG_MODE) Serial.print("enA value: ");
  if(DEBUG_MODE) Serial.println(getPWMValue(enA));

  digitalWrite(PIN_IN1, in1);
  digitalWrite(PIN_IN2, in2);

  // Turn enable back on
  analogWrite(PIN_ENA, getPWMValue(enA));

  /** RIGHT WHEEL **/

  // Turn off enable momentarily so we don't accidentally brake during switching
  if(DEBUG_MODE) Serial.print("enB value: ");
  if(DEBUG_MODE) Serial.println(getPWMValue(enB));

  digitalWrite(PIN_IN3, in3);
  digitalWrite(PIN_IN4, in4);

  // Turn enable back on
  analogWrite(PIN_ENB, getPWMValue(enB));
}

void setAllPins() {
  setPins(pinStates[currentState][0],
            pinStates[currentState][1],
            pinStates[currentState][2],
            pinStates[currentState][3],
            pinStates[currentState][4],
            pinStates[currentState][5],
            currentState);
}

int getPWMValue(uint8_t isEnabled) {
  return (int) (255.0 * currentSpeed * isEnabled);
}

void forward()    { currentState = FORWARD;     } 
void reverse()    { currentState = REVERSE;     } 
void brake()      { currentState = BRAKE;       }
void coast()      { currentState = COAST;       }
void turnLeft()   { currentState = TURN_LEFT;   }
void turnRight()  { currentState = TURN_RIGHT;  }
void pivotLeft()  { currentState = PIVOT_LEFT;  }
void pivotRight() { currentState = PIVOT_RIGHT; }

void awaitDistance(int cmDistance) {
  double startingDistance = getDistance();
  double metersDistance = ((double) cmDistance) / 100.0;
  
  while ((getDistance() - startingDistance) < metersDistance) {
    if(DEBUG_MODE) Serial.print("Distance travelled: ");
    if(DEBUG_MODE) Serial.print(getDistance() - startingDistance);
    if(DEBUG_MODE) Serial.print(", Distance needed: ");
    if(DEBUG_MODE) Serial.print(metersDistance);
    if(DEBUG_MODE) Serial.print(", Counter: ");
    if(DEBUG_MODE) Serial.println(counter);
  }

  currentState = BRAKE;
  setAllPins();

  return;
}

void cmForward(int x) {
  currentState = FORWARD;
  setAllPins();
  awaitDistance(x);
}

void cmReverse(int x) {
  currentState = REVERSE;
  setAllPins();
  awaitDistance(x);
}

void readChannelA() {
 currChannelATime = millis();
 // Check whether B's pulse was closer to this pulse or the last one.
 // If it is closer to this pulse, we are moving from B to A
 // If it is closer to the last pulse, we are millis() - lastChannelBTime moving from A to B
 long d2 = lastChannelBTime - lastChannelATime;
 long d1 = currChannelATime - lastChannelBTime;
 if(d1 > d2) {
   counter++;
 } else {
   counter--;
 }

 lastChannelATime = currChannelATime;

 // Update stored distance value
 totalDistance = getDistance();
}

void readChannelB() {
 lastChannelBTime = millis();
}

double getDistance() {
  return (double) counter / countsPerRotation * circumference;
}

void printDistance() {
  Serial.println(getDistance());
}

void printPins() {
  Serial.print("Current pin states are: ");
  for(int i = 0; i < SWITCH_COUNT; i++) {
    Serial.print(digitalRead(switchPins[i]));
    Serial.print(", ");
  }
}

void setup() {
  Serial.begin(9600);

  // Configure outputs
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  // Configure inputs for distance
  pinMode(CHANNEL_A, INPUT);
  pinMode(CHANNEL_B, INPUT);
  
  for(int i = 0; i < SWITCH_COUNT; i++) {
    pinMode(switchPins[i], INPUT);
  }

  // Configure interrupt
  attachInterrupt(CHANNEL_A, readChannelA, CHANGE);
  attachInterrupt(CHANNEL_B, readChannelB, CHANGE);

  forward();
  // setTimeout(&coast, 2000);
  if(DEBUG_MODE) setInterval(&printDistance, 2000);
  if(DEBUG_MODE) setInterval(&printPins, 2000);
}

void loop() {
  // Add logic here
  readTimeouts();

  // Set speed
  if(digitalRead(switchPins[0]) == HIGH)      currentSpeed = 1;
  else if(digitalRead(switchPins[1]) == HIGH) currentSpeed = 0.60;
  else if(digitalRead(switchPins[2]) == HIGH) currentSpeed = 0.40;
  else if(digitalRead(switchPins[3]) == HIGH) currentSpeed = 0.25;

  if(previousState != currentState || previousSpeed != currentSpeed) {
    if(DEBUG_MODE) Serial.print("Changing state! Previous state: ");
    if(DEBUG_MODE) Serial.print(previousState);
    if(DEBUG_MODE) Serial.print(", Current State: ");
    if(DEBUG_MODE) Serial.println(currentState);

    // Set pin values
    setAllPins();
  }

  previousState = currentState;
  previousSpeed = currentSpeed;
}
