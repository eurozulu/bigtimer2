#include "bigtimer2.h"

void setup() {
  Serial.begin(38400);

  pinMode(PIN_A, OUTPUT);
  //pinMode(PIN_B, OUTPUT);

  Serial.println("Starting frequency generator on Timer2");
  Serial.println("Send 'f' followed by a frequency, in hz to serial to start timer");
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

void loop() {

  // If serial has something to say, go see what it is
  if (Serial.available()) {
    char buf[255];
    int len = parseSerial(buf, 255);
    if (len < 2)
      return;

    switch (buf[0]) {
      case 'f' :
        char *s = &buf[1];
        int hz = atoi(s);
        Serial.print("Setting frequency to ");
        Serial.println(hz);
        BigTimer.startTimerFrequency((uint16_t)hz);
        break;
        
      default:
        Serial.print(&buf[1]);
        Serial.print(" is an unknown command");
        Serial.println();
    }
  }
}
