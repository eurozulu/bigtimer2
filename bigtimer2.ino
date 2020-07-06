#include "bigtimer2.h"

BigTimer2 *bigTimer = new BigTimer2();

void setFrequency(uint16_t hz) {
  Serial.print("Setting frequency to ");
  Serial.print(hz);
  Serial.println(" hz");
  bigTimer->startTimerFrequency(hz);
}

int parseSerial(char cBuf[], int sz) {
  int len = 0;
  while (len < (sz - 1) && Serial.available() > 0) {
    cBuf[len] = (char)Serial.read();
    len++;
    if (Serial.available() == 0) // small delay to allow next char to arrive if not already.
      delay(100);
  }
  cBuf[len] = 0; // mark end of string
  len++;

  return len;
}

void setup() {
  Serial.begin(38400);

  Serial.println("Starting frequency generator on Timer2");
  Serial.println("Send 'f' followed by a frequency, in hz, to serial to change the timer frequency.");
  Serial.println("e.g. f1000 will set to pulse at 1 khz.  f2 will pulse at 2 hz");

  setFrequency(1);
}


// Note, Timer itself does nothing in the main loop as it's all hardware driven.
// Code here is simply to read Serial and control the frequency.
void loop() {

  // If serial has something to say, go see what it is
  if (Serial.available()) {
    char buf[255];
    int len = parseSerial(buf, 255);
    if (len < 2)
      return;

    switch (buf[0]) {
      case 'f' :
        int hz = atoi(&buf[1]); // Grab string following 'f' command adn flip to number. zero if NAN.
        setFrequency((uint16_t) hz);
        break;

      default:
        Serial.print(&buf[1]);
        Serial.print(" is an unknown command");
        Serial.println();
    }
  }
}



ISR(TIMER2_COMPA_vect) {
  bigTimer->CompareMatchA();
}

ISR(TIMER2_OVF_vect) {
  bigTimer->Overflow();
}
