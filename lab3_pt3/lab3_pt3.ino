#include "TimerOne.h"
#define BUTTON_PIN 2

volatile int counter = 0;
int timerDelay = 10000;

volatile bool isPrintEnabled = true;
volatile bool isPrinting = false;

void setup() {
  Timer1.initialize(timerDelay);
  Timer1.attachInterrupt(handleTimer);

  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButton, RISING);
}

void handleTimer() {
  //Serial.print("hi");
  if(counter >= 100 && isPrintEnabled) {
    counter = 0;
    isPrinting = true;
  }
  counter++;
}

void handleButton() {
  isPrintEnabled = !isPrintEnabled;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(isPrinting) {
    Serial.print(millis());
    Serial.println(" ms elapsed.");
    isPrinting = false;
  }
}
