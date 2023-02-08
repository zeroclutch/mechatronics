#define COUNT 8
#define DEBUG 0
#define BUTTON_PIN 2
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


void setup() {
  // Configure pins
  pinMode(A0, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Configure serial port
  Serial.begin(9600);
}

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

void isr0() {
  int value = analogRead(A0);
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

}

bool isPressed = false;

void loop() {
  if(digitalRead(BUTTON_PIN) == 1 && !isPressed) {
    isPressed = true;
    isr0();
    delay(100);
  }
  
  if(digitalRead(BUTTON_PIN) == 1 && isPressed) {
    isPressed = false;
  }


}