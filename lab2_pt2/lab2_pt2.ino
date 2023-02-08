/*
5. Now, write a program to generate a 1KHz square wave
on pin 12 of the Arduino. Observe this signal on the Oscilloscope
on channel 1. Record what you see. (Cell phone picture is easiest –
but you can do a drawing if you want. You should generally record
scope outputs for your report. If you bring a thumb drive, the
scope can take screenshots)
*/
#include "timeouts.h"

void setup() {
  // Configure pins
  pinMode(12, OUTPUT);
}

int pinValue = HIGH;

void toggle() {
  if(pinValue == HIGH) {
    pinValue = LOW;
  } else {
    pinValue = HIGH;
  }

  digitalWrite(12, pinValue);
}

void loop() {
  delayMicroseconds(500);
  toggle();
}