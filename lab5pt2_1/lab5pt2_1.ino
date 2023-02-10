#define RED 44
#define BLUE 46
#define GREEN 42
#define TIMEINTERVAL 10
#define PHOTOIN A0

#define SAMPLE_COUNT 100

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

void printLED(int reading[]) {
  for (i = 0; i < SAMPLE_COUNT; i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.println(reading[i]);
  }
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

  sampleLED(RED, redReading);
  delay(100);
  sampleLED(GREEN, greenReading);
  delay(100);
  sampleLED(BLUE, blueReading);

  printLED(redReading);
  printLED(greenReading);
  printLED(blueReading);
  
}

void loop() {

}
