# define CHANNEL_A 2
# define CHANNEL_B 3

volatile long counter = 0;

volatile long lastChannelATime = 0;
volatile long currChannelATime = 0;
volatile long lastChannelBTime = 0;

bool hasPrinted = true;

void setup() {
  // Configure pins
  pinMode(CHANNEL_A, INPUT);
  pinMode(CHANNEL_B, INPUT);

  Serial.begin(9600);

  // Configure ISRs
  attachInterrupt(digitalPinToInterrupt(CHANNEL_A), readChannelA, RISING);
  attachInterrupt(digitalPinToInterrupt(CHANNEL_B), readChannelB, RISING);
}

void readChannelA() {
  currChannelATime = millis();
  // Check whether B's pulse was closer to this pulse or the last one.
  // If it is closer to this pulse, we are moving from B to A
  // If it is closer to the last pulse, we aremillis() - lastChannelBTime moving from A to B
  long d2 = lastChannelBTime - lastChannelATime;
  long d1 = currChannelATime - lastChannelBTime;
  if(d1 > d2) {
    counter++;
  } else {
    counter--;
  }

  hasPrinted = false;
  lastChannelATime = currChannelATime;
}

void readChannelB() {
  lastChannelBTime = millis();
}

void loop() {
  if(!hasPrinted) {
    Serial.println(counter);
  }
}
