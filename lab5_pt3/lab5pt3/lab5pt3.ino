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
#define CALIBRATION_MODE 1
#define DEBUG 1

#define RED 44
#define BLUE 46
#define GREEN 42
#define WHITE 40
#define TIMEINTERVAL 1000
#define PHOTOIN A0

#define RED_LED 8
#define GREEN_LED 9
#define BLUE_LED 10
#define WHITE_LED 11

#define SAMPLE_COUNT 20

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

enum colorCodes {
  CODE_RED,
  CODE_BLUE,
  CODE_GREEN,
  CODE_WHITE,
  CODE_BLACK,

  // Unofficial colors
  CODE_ORANGE,
  CODE_YELLOW,
  CODE_PURPLE,
  CODE_PINK,
};

typedef struct rgb {
  float r, g, b;
} RGB;

typedef struct hsl {
  float h, s, l;
} HSL;

/******* CHANGE EVERYTHING BELOW *******/

// How to calibrate:
// FOR RGB:
//  1. Find the maximum R, G, B, W values for each color sheet.
//  2. Put those values into maxR, maxG, maxB, maxW, respectively.
// FOR HSL:
//  1. Approximate the sRGB value and put it into the calibrationRGB struct
//  2. Record the coefficients logged. Note those down.
//  3. Do this for every possible color sheet
//  4. Average the coefficients out and put them in rChange, gChange, bChange

// This is the RGB value we are estimating for a given object
RGB calibrationRGB = {
  r: 52.0,
  g: 89.0,
  b: 128.0
};

// GREEN: 65, 140, 76
// BLUE 52, 89, 128
// RED 230, 86, 76

// Coefficients (trueRgb/ourRgb)

float rChange = 1.10;
float gChange = 1.94;
float bChange = 1.16;

// srgb of object -> our rgb coordinates
// our rgb coordinates -> srgb -> hsl -> classify

const float maxR = 140.0;
const float maxG = 80.8;
const float maxB = 105.0;
const float maxW = 130;

/******* CHANGE EVERYTHING ABOVE *******/

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
    delayMicroseconds((unsigned long)(TIMEINTERVAL));

    // Record the phototransistor reading
    reading[i] = analogRead(PHOTOIN);
    digitalWrite(LED, LOW);

    // Wait 100ms before taking another sample
    delay(10);
  }
}

float getValues(int reading[]) {
  int sum = 0;
  for (i = 0; i < SAMPLE_COUNT; i++) {
    sum += reading[i];
  }
  return ((float) sum) / SAMPLE_COUNT / 1024 * 255.0;
}

float clamp(float x, float min, float max) {
  return MAX(MIN(x, max), min);
}

void resolveColor(int color) {
  switch(color) {
    case CODE_RED: 
      Serial.println("Red!");
      digitalWrite(RED_LED, HIGH);
      break;
    case CODE_GREEN: 
      Serial.println("Green!");
      digitalWrite(GREEN_LED, HIGH);
      break;
    case CODE_BLUE: 
      Serial.println("Blue!");
      digitalWrite(BLUE_LED, HIGH);
      break;
    case CODE_WHITE: 
      Serial.println("White!");
      digitalWrite(WHITE_LED, HIGH);
      break;
    case CODE_BLACK: 
      Serial.println("Black!");
      break;
    case CODE_ORANGE: 
      Serial.println("Orange!");
      break;
    case CODE_YELLOW: 
      Serial.println("Yellow!");
      break;
    case CODE_PURPLE: 
      Serial.println("Purple!");
      break;
    case CODE_PINK: 
      Serial.println("Pink!");
      break;
    default:
      Serial.println("An unknown color was provided...");
  }
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

  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(WHITE_LED, LOW);

  analogReference(DEFAULT);

  pinMode(PHOTOIN, INPUT);

  sampleLED(RED, redReading);
  delay(10);
  sampleLED(GREEN, greenReading);
  delay(10);
  sampleLED(BLUE, blueReading);

  int r = getValues(redReading);
  int g = getValues(greenReading);
  int b = getValues(blueReading);
  int w = (r + g + b)/3;

  if(DEBUG) Serial.print("r: ");
  if(DEBUG) Serial.println(r);
  if(DEBUG) Serial.print("g: ");
  if(DEBUG) Serial.println(g);
  if(DEBUG) Serial.print("b: ");
  if(DEBUG) Serial.println(b);
  if(DEBUG) Serial.print("w: ");
  if(DEBUG) Serial.println(w);

  if(DEBUG) Serial.print("analog r: ");
  if(DEBUG) Serial.println((int) r * 1024.0 / 255.0 );
  if(DEBUG) Serial.print("analog g: ");
  if(DEBUG) Serial.println((int) g * 1024.0 / 255.0  );
  if(DEBUG) Serial.print("analog b: ");
  if(DEBUG) Serial.println((int) b * 1024.0 / 255.0 );
  if(DEBUG) Serial.print("analog w: ");
  if(DEBUG) Serial.println((int) w * 1024.0 / 255.0 );

  if(CALIBRATION_MODE) {
    rChange = calibrationRGB.r / r;
    gChange = calibrationRGB.g / g;
    bChange = calibrationRGB.b / b;
    Serial.println("\n\nCalibration values:");
    Serial.print("rChange = ");
    Serial.print(rChange);
    Serial.print(";\ngChange = ");
    Serial.print(gChange);
    Serial.print(";\nbChange = ");
    Serial.print(bChange);
    Serial.println(";\n\n");
  }

  int color1 = discriminateByRGB(r, g, b, w);
  int color2 = discriminateByHSL(r,g,b,w);

  Serial.println("Using RGB formula!");
  resolveColor(color1);

  // Serial.println("Using HSL formula!");
  // resolveColor(color2);
}

int discriminateByRGB(float r, float g, float b, float w) {
  // Normalize color 
  // Note, real normalization would also consider minR,
  // we are assuming that to be zero, but that may not
  // be the case.
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
  const float cutoffForBlack = 0.75;

  // Check black/white
  if(percentW > 1) {
    return CODE_WHITE;
  } 

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

HSL rgb2hsl(float r, float g, float b) {
  
  HSL result;
  
  r /= 255;
  g /= 255;
  b /= 255;
  
  float max = MAX(MAX(r,g),b);
  float min = MIN(MIN(r,g),b);
  
  result.h = result.s = result.l = (max + min) / 2;

  if (max == min) {
    result.h = result.s = 0; // achromatic
  }
  else {
    float d = max - min;
    result.s = (result.l > 0.5) ? d / (2 - max - min) : d / (max + min);
    
    if (max == r) {
      result.h = (g - b) / d + (g < b ? 6 : 0);
    }
    else if (max == g) {
      result.h = (b - r) / d + 2;
    }
    else if (max == b) {
      result.h = (r - g) / d + 4;
    }
    
    result.h /= 6;
  }

  return result;
  
}

int discriminateByHSL(float r, float g, float b, float w) {
  r = clamp(r * rChange, 0, 255);
  g = clamp(g * gChange, 0, 255);
  b = clamp(b * bChange, 0, 255);

  // Multiply by coefficients
  HSL hslValue = rgb2hsl(r, g, b);
  
  // Filter out grays and whites
  if(hslValue.s < 0.5 && hslValue.l < 0.2) {
    return CODE_BLACK;// TODO: change these as needed
  }

  if(hslValue.s < 0.5 && hslValue.l > 0.9) {
    return CODE_WHITE; // TODO: change these as needed
  }
  
  float h = hslValue.h;
  // Get color
  if(h <= 0.04)  return CODE_RED;
  if(h <= 0.11)  return CODE_ORANGE;
  if(h <= 0.175) return CODE_YELLOW;
  if(h <= 0.46)  return CODE_GREEN;
  if(h <= 0.69)  return CODE_BLUE;
  if(h <= 0.77)  return CODE_PURPLE;
  if(h <= 0.96)  return CODE_PINK;
  return CODE_RED;
}

void loop() {}
