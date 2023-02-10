/**
7. Add a red, green, blue, and white LED to your design (from your kit). Connect each one to an
Arduino digital output on one side and through a 330Ω resistor to ground on the other.
8. Create a discrimination function that determines if a paper sheet is:
a. Red
b. Green
c. Blue
d. Black
e. White
And turns on the corresponding LED to indicate which. (No LED for black)
**/

#define RED 44
#define BLUE 46
#define GREEN 42
#define WHITE 40
#define TIMEINTERVAL 10
#define PHOTOIN A0

#define SAMPLE_COUNT 4

#define DEBUG 1
#define CIVIL_RIGHTS_ACT 0

enum colorCodes {
  CODE_RED,
  CODE_BLUE,
  CODE_GREEN,
  CODE_WHITE,
  CODE_BLACK
};

// CHANGE THIS BASED ON READ VALUES
const int maxR = 1024 * 4;
const int maxG = 1024 * 4;
const int maxB = 1024 * 4;
const int maxW = 1024 * 4;

int i;

int redReading[SAMPLE_COUNT];
int greenReading[SAMPLE_COUNT];
int blueReading[SAMPLE_COUNT];
int whiteReading[SAMPLE_COUNT];

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
  return sum;
}

void setup() {
  Serial.begin(9600);

  // Configure pins
  pinMode(RED, OUTPUT);
  digitalWrite(RED, LOW);
  pinMode(BLUE, OUTPUT);
  digitalWrite(BLUE, LOW);
  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, LOW);
  pinMode(WHITE, OUTPUT);
  digitalWrite(WHITE, LOW);

  analogReference(DEFAULT);

  pinMode(PHOTOIN, INPUT);

  sampleLED(RED, redReading);
  delay(10);
  sampleLED(GREEN, greenReading);
  delay(10);
  sampleLED(BLUE, blueReading);
  delay(10);
  sampleLED(BLUE, whiteReading);

  int r = getValues(redReading);
  int g = getValues(greenReading);
  int b = getValues(blueReading);
  int w = getValues(whiteReading);

  if(DEBUG) Serial.print("r: ");
  if(DEBUG) Serial.println(r);
  if(DEBUG) Serial.print("g: ");
  if(DEBUG) Serial.println(g);
  if(DEBUG) Serial.print("b: ");
  if(DEBUG) Serial.println(b);
  if(DEBUG) Serial.print("w: ");
  if(DEBUG) Serial.println(w);

  int color = discriminateByColor(r, g, b, w);

  switch(color) {
    case CODE_RED: 
      Serial.println("Red!");
      break;
    case CODE_GREEN: 
      Serial.println("Green!");
      break;
    case CODE_BLUE: 
      Serial.println("Blue!");
      break;
    case CODE_WHITE: 
      Serial.println("White!");
      break;
    case CODE_BLACK: 
      Serial.println("Black!");
      break;
    case -1:
      Serial.println("no discriminating allowed");
      break;
    default:
      Serial.println("An unknown color was provided...");
  }
}

int discriminateByColor(int r, int g, int b, int w) {
  if(CIVIL_RIGHTS_ACT) { return -1; }

  // Normalize color
  float percentR = ((float) r) / ((float) maxR);
  float percentG = ((float) g) / ((float) maxG);
  float percentB = ((float) b) / ((float) maxB);
  float percentW = ((float) w) / ((float) maxW);


  if(DEBUG) Serial.print("percentR: ");
  if(DEBUG) Serial.println(percentR);
  if(DEBUG) Serial.print("percentG: ");
  if(DEBUG) Serial.println(percentG);
  if(DEBUG) Serial.print("percentB: ");
  if(DEBUG) Serial.println(percentB);
  if(DEBUG) Serial.print("percentW: ");
  if(DEBUG) Serial.println(percentW);

  // If no color reaches 25% of its maximum, say it's black
  const float cutoffForBlack = 0.25;

  // Check black
  if(firstIsBiggest(cutoffForBlack, percentR, percentG, percentB, percentW)) {
    return CODE_BLACK;
  }

  if(firstIsBiggest(percentR, percentG, percentB, percentW, 0)) {
    return CODE_RED;
  }

  if(firstIsBiggest(percentG, percentB, percentW, 0, 0)) {
    return CODE_GREEN;
  }

  if(firstIsBiggest(percentB, percentW, 0, 0, 0)) {
    return CODE_BLUE;
  }

  return CODE_WHITE;

}

// Checks to see if the first argument has the largest value
bool firstIsBiggest(float a, float b, float c, float d, float e) {
  return a > b && a > c && a > d && a > e;
}

void loop() {}
