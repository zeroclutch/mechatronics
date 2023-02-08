#define LED_COUNT 8
#define SWITCH_COUNT 4

#define BLINKING_LED_INDEX 4

int ledPins[LED_COUNT] = {30, 31, 32, 33, 34, 35, 36, 37};
int switchPins[SWITCH_COUNT] = {9, 10, 11, 12};


void setup() {
  for(int i = 0; i < LED_COUNT; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  for(int i = 0; i < SWITCH_COUNT; i++) {
    pinMode(switchPins[i], INPUT);
  }
}

bool lightUp = false;
unsigned long nextTime = 0;

void loop() {
  // Update first 4 leds
  for(int i = 0; i < SWITCH_COUNT; i++) {
    digitalWrite(ledPins[i], digitalRead(switchPins[i]));   // read the input pin
  }

  // Update next 4 leds
  for(int i = 0; i < SWITCH_COUNT; i++) {
    // Blinking light index is the index of the first LED that needs to blink
    int pin = ledPins[i + BLINKING_LED_INDEX];

    // Check if pin is lit up
    if(lightUp) {
      digitalWrite(pin, digitalRead(switchPins[i]));   // read the input pin
    } else {
      digitalWrite(pin, LOW);   // read the input pin
    }
  }

  if(millis() > nextTime) {
    nextTime = millis() + 500;
    lightUp = !lightUp;
  }
}

