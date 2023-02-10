#include <Wire.h>
#include "Adafruit_TCS34725.h"

/** Part 2: 
Modify the demo program to set the measurement
time to 24ms and the measurement gain to 16X. (Hint: Find the source code for
Adafruit_TCS34725.h on your computer.) Record any observations as to how this changes the
behavior and accuracy of the sensor.
**/

/* Example code for the Adafruit TCS34725 breakout library */

/* WARNING: MAKE SURE POWER CONNECTION IS WHAT WOLFE IS SPECIFYING! -PG
   Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC 
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);

void setup(void) {
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Now we're ready to get readings!
}

void loop(void) {
  unsigned long startTime = millis();
  uint16_t r, g, b, c, colorTemp, lux;

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  // Report loop time
  Serial.print("Time elapsed: ");
  Serial.print(millis() - startTime);
  Serial.println(" ");
}
