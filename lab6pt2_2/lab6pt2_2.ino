#define DIR_PIN 13
#define STEP_PIN 12

void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
}

int stepCount = 0;
int dir = LOW;

void loop() {

  digitalWrite(STEP_PIN, HIGH);
  delay(1);
  digitalWrite(STEP_PIN, LOW);
  delay(1);

  
  stepCount++;

  if(stepCount % 384 == 0) {
    delay(1000);
    dir = !dir;
    digitalWrite(DIR_PIN, dir);
  }

}


/*
1. Record the waveform between motor terminals A+ and A- to ground using DC coupling on the
scope.
2. From the waveform, determine when the stepper motor consumes the most power.
3. Modify the program to run the motor 384 microsteps forward, wait 1 second, then 384 microsteps
backwards. Place some tape on the shaft and measure the amount of rotation that corresponds to
384 microsteps. Verify or disprove my theory that the motor requires 384 microsteps per
revolution. (Demo #2 – show the motor running this program)
*/