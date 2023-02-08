#define LED_COUNT 8
#define SWITCH_COUNT 4

#define BLINKING_LED_INDEX 4

int ledPins[LED_COUNT] = {30, 31, 32, 33, 34, 35, 36, 37};
int switchPins[SWITCH_COUNT] = {9, 10, 11, 12};


void setup() {
  Serial.begin(9600);
  
  for(int i = 0; i < SWITCH_COUNT; i++) {
    pinMode(switchPins[i], INPUT);
  }
}

bool lightUp = false;
unsigned long nextTime = 0;

void loop() {
  // Update first 4 leds
  for(int i = 0; i < SWITCH_COUNT; i++) {
    Serial.print("Switch ");
    Serial.print(i + 1);
    Serial.print(" is ");
    Serial.print(!digitalRead(switchPins[i]));   // read the input pin
    Serial.print(". ");
  }
  Serial.println("");
  delay(100);
}

