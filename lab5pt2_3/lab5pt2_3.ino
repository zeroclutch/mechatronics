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
#define TIMEINTERVAL 10
#define PHOTOIN A0

#define SAMPLE_COUNT 4

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

enum colorCodes {
  CODE_RED,
  CODE_BLUE,
  CODE_GREEN,
  CODE_WHITE,
  CODE_BLACK
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
  r: 255.0,
  g: 100.0,
  b: 0.0
};

// Coefficients (trueRgb/ourRgb)
float rChange = 1.0;
float gChange = 1.0;
float bChange = 1.0;

// srgb of object -> our rgb coordinates
// our rgb coordinates -> srgb -> hsl -> classify

const float maxR = 255;
const float maxG = 255;
const float maxB = 255;
const float maxW = 255;

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
    delayMicroseconds((unsigned long)(i * TIMEINTERVAL));

    // Record the phototransistor reading
    reading[i] = analogRead(PHOTOIN);
    digitalWrite(LED, LOW);

    // Wait 100ms before taking another sample
    delay(100);
  }
}

float getValues(int reading[]) {
  int sum = 0;
  for (i = 0; i < SAMPLE_COUNT; i++) {
    sum += reading[i];
  }
  return ((float) sum) / SAMPLE_COUNT / 1024 * 255.0;
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

  if(CALIBRATION_MODE) {
    rChange = calibrationRGB.r;
    gChange = calibrationRGB.g;
    bChange = calibrationRGB.b;
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
  switch(color1) {
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
    default:
      Serial.println("An unknown color was provided...");
  }

  Serial.println("Using HSL formula!");
  switch(color2) {
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
    default:
      Serial.println("An unknown color was provided...");
  }
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
  // Multiply by coefficients
  HSL hslValue = rgb2hsl(r * rChange, g * gChange, b * bChange);
  
  // Filter out grays and whites
  if(w < 50) {
    return CODE_BLACK;// TODO: change these as needed
  }

  if(w > 220) {
    return CODE_WHITE; // TODO: change these as needed
  }
  
  float h = hslValue.h;
  // Get color
  if(h <= 0.17) {
    return CODE_RED;
  } else if (h < 0.46) {
    return CODE_GREEN;
  } else if (h < 0.77) {
    return CODE_BLUE;
  } else {
    return CODE_RED;
  }
}

void loop() {}
