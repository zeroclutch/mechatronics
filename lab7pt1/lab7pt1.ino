#include <Wire.h>
#include "/Users/pranav/Documents/Arduino/mechatronics/Labs/lab7_init_waveTable/lab7_init_waveTable.ino"
#define DEBUG 0
  
const int WIRE_SPEED = 400000;

int inval;
int phase;
unsigned long lastPrintTime;

int songIndex = 1;
unsigned long lastNoteTime;

bool dacDisable = false;
unsigned long lastDelayTime;
int dacDelay = 100;


int frequency = 180;
const int SAMPLE_RATE = 20;
const int PHASEINC = ((64 * 256)/SAMPLE_RATE);

const int BPM = 10;
// const int MICROS_DELAY = 278;

enum tones {
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  S
};

enum lengths {
  quarter = 400,
  half = 800,
  dotted = 1200,
  whole = 1600,
};

int notes[8][3] = {
 { 55, 110, 220 }, // A
 { 62, 123, 247 }, // B
 { 65, 130, 262 }, // C
 { 73, 147, 294 }, // D
 { 82, 165, 330 }, // E
 { 87, 175, 349 }, // F
 { 98, 196, 392 }, // G
 { 1,  1,   1 }  // S
};

int song[][2] = {
  { notes[S][0], quarter },
  { notes[B][2], quarter },
  { notes[A][2], quarter },
  { notes[G][1], quarter },
  { notes[A][2], quarter },
  { notes[B][2], quarter },
  { notes[B][2], quarter },
  { notes[B][2], half    },
  { notes[A][2], quarter },
  { notes[A][2], quarter },
  { notes[A][2], half    },
  { notes[B][2], quarter },
  { notes[D][2], quarter },
  { notes[D][2], half    },
  { notes[B][2], quarter },
  { notes[A][2], quarter },
  { notes[G][1], quarter },
  { notes[A][2], quarter },
  { notes[B][2], quarter },
  { notes[B][2], quarter },
  { notes[B][2], quarter },
  { notes[B][2], quarter },
  { notes[A][2], quarter },
  { notes[A][2], quarter },
  { notes[B][2], quarter },
  { notes[A][2], quarter },
  { notes[G][1], whole },

};
  
int wave(int phase) {
    if ((phase < 0) || (phase >= (256 * 64))) {
     return 0;
   }
   return waveTable[phase/64];  
}

void setDAC(unsigned char dacval) {
  Wire.beginTransmission(0x2C); 
  // Send command byte (0x00)
  Wire.write(0x00); // for each byte
  Wire.write(dacval); // for each byte
  // Sends the message
  Wire.endTransmission();
}

void setup() {
  phase = 0;
  lastPrintTime = 0;
  init_waveTable();
  if(DEBUG) Serial.begin(9600);

  Wire.setClock(400000);
}


void loop_part_1() {
  setDAC(0x65);
}

// unsigned int getMillisDelay(int note) {
//   return (note * 1000 * 60) / BPM;
// }

unsigned int getFrequencyDelay() {
  return 1000000 / (frequency * SAMPLE_RATE);
}

void mytone(int freq, long tonelength)  {
  
}

void loop() {
  unsigned int frequencyDelay = getFrequencyDelay();
  if(DEBUG) Serial.print(", frequency delay: ");
  if(DEBUG) Serial.print(frequencyDelay);
  if ((micros()-lastPrintTime) > frequencyDelay) {
    // if(DEBUG) Serial.println(wave(phase));
    if(dacDisable) setDAC(0);
    else setDAC(wave(phase)/4);

    lastPrintTime = lastPrintTime + frequencyDelay;
    phase = phase + PHASEINC;
    if (phase >= (256 * 64)) {
      phase = phase - (256*64);   
    }
  }

  int *currentNote = song[songIndex];
  frequency = currentNote[0];
  
  unsigned int millisDelay = currentNote[1];
  
  if(DEBUG) Serial.print(", delay to next note: ");
  if(DEBUG) Serial.print(millisDelay);
  if((millis() - lastNoteTime) > millisDelay) {
    songIndex++;
    dacDisable = true;
    lastDelayTime = millis();
    lastNoteTime = lastNoteTime + millisDelay ;
  }

  if((millis() - lastDelayTime) > dacDelay) {
    dacDisable = false;
  }
  
  if(songIndex > 28) songIndex = 0;
  if(DEBUG) Serial.print(", songindex: ");
  if(DEBUG) Serial.println(songIndex);
}
