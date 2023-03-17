/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

#define COUNT 8
#define DEBUG 0
#define BUTTON_PIN 2

#define MAX_ANALOG_READ 613
#define DISTANCE_SENSOR_PIN A1
#define SAMPLE_COUNT 20

int samples[SAMPLE_COUNT];
int sampleIndex = 0;

/*
Measures the analog input voltage from the sensor.
b) Uses the information you gathered in the prior experiment to calculate the distance to
the wall. Use a piecewise linear model to approximate the conversion function.
c) The program should print the distance to the wall on the serial console each time
you press the pushbutton. Report the distance to the nearest 1mm when possible.
DEMO #1 – show the TA that this program works.
*/

// Sorted list of input/output pairs
float dataPoints[COUNT][2] = {
  {3.065, 5},
  {2.191, 10},
  {1.695, 15},
  {1.338, 20},
  {1.066, 25},
  {0.933, 30},
  {0.775, 35},
  {0.687, 40}
};


float calculateDistance(float voltage) {
  // Implement piecewise linear function
  int i;
  for(i = 0; i < COUNT - 1; i++) {
    // See if next value is larger
    if(voltage > dataPoints[i + 1][0]) {
      // Calculate value
      float x1 = (float) dataPoints[i][0];
      float y1 = (float) dataPoints[i][1];
      float x0 = (float) dataPoints[i + 1][0];
      float y0 = (float) dataPoints[i + 1][1];

      // Point-slope form y = y0 + m(x - x0)
      return y0 + (y1-y0)/(x1-x0) * (voltage - x0);
    }
  }
  return -1;
}

int readValue() {
  int value = analogRead(DISTANCE_SENSOR_PIN);
  float voltage = value * (5.0 / 1023.0);
  Serial.print("The distance is approximately: ");
  Serial.print(calculateDistance(voltage));
  Serial.println("cm.");
  if(DEBUG) {
    Serial.print("Raw value: ");
    Serial.println(value);
    Serial.print("Voltage: ");
    Serial.println(voltage);
  }
  return value;
}

void logSample(int value) {
  samples[sampleIndex] = value;
  sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT;
}

int averageSample() { 
  int sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += samples[i];
  }
  return sum/SAMPLE_COUNT;
}

Servo myservo;  // create servo object to control a servo


void setup() {
  myservo.attach(11);  // attaches the servo on pin 9 to the servo object

  // Configure pins
  pinMode(DISTANCE_SENSOR_PIN, INPUT);

  // Configure serial port
  Serial.begin(9600);
}


void loop() {
  int analogValue = MAX_ANALOG_READ - readValue();
  logSample(analogValue);
  analogValue = map(averageSample(), 0, MAX_ANALOG_READ, 0, 340);     // scale it for use with the servo (value between 0 and 180)
  myservo.write(analogValue);                  // sets the servo position according to the scaled value

  delay(15);                           // waits for the servo to get there
}
