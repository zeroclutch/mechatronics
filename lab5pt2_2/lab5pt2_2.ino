/**
5. Write a program that every 100ms:
  a. Turns on the Red LED, waits until it is at least 90% brightness; reads the phototransistor
    voltage level 4 times and adds the 4 results to produce a value r; turns off the red LED for
    at least 1ms.
  b. Turns on the Green LED, waits until it is at least 90% brightness; reads the
    phototransistor voltage level 4 times and adds the 4 results to produce a value g; turns off
    the red LED for at least 1ms.
  c. Turns on the Blue LED, waits until it is at least 90% brightness; reads the phototransistor
    voltage level 4 times and adds the 4 results to produce a value b; turns off the red LED
    for at least 1ms.
  d. Prints the values of r, g, and b to the Serial Monitor with labels.

NOTE: Remember to log these values for the next part!
**/

#define RED 44
#define BLUE 46
#define GREEN 42
#define TIMEINTERVAL 10
#define PHOTOIN A0

#define SAMPLE_COUNT 4

#define DEBUG 1

int i;

int redReading[SAMPLE_COUNT];
int greenReading[SAMPLE_COUNT];
int blueReading[SAMPLE_COUNT];

void sampleLED(int LED, int reading[]) {
  if(DEBUG) Serial.print("Sampling LED at pin ");
  if(DEBUG) Serial.println(LED);

  reading[0] = analogRead(PHOTOIN);

  // Take 100 samples of the LED
  for (i = 0; i < SAMPLE_COUNT; i++) {
    // Enable the LED
    digitalWrite(LED, HIGH);
    delayMicroseconds((unsigned long)(i * TIMEINTERVAL));

    // Record the phototransistor reading
    reading[i] = analogRead(PHOTOIN);
    digitalWrite(LED, LOW);

    // Wait 100ms before taking another sample
    delay(100);
  }
}

int getValues(int reading[]) {
  int sum = 0;
  for (i = 0; i < SAMPLE_COUNT; i++) {
    sum += reading[i];
  }
  return sum
}

void setup() {
  Serial.begin(9600);
  // Configure pins
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(BLUE, LOW);
  analogReference(DEFAULT);

  pinMode(PHOTOIN, INPUT);

  // Sample each LED
  sampleLED(RED, redReading);
  delay(10);
  sampleLED(GREEN, greenReading);
  delay(10);
  sampleLED(BLUE, blueReading);

  // Sum the values
  int r = getValues(redReading);
  int g = getValues(greenReading);
  int b = getValues(blueReading);

  // Print to serial
  Serial.print("r: ");
  Serial.println(r);
  Serial.print("g: ");
  Serial.println(g);
  Serial.print("b: ");
  Serial.println(b);
  
}

void loop() {

}
