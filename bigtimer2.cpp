#include "bigtimer2.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Helper to start timer with a given frequency.
// Calculates a 16-bit counter and prescaler to achive this frequency
void BigTimer2::startTimerFrequency(uint16_t hz) {
  if (hz == 0) {
    return;
  }
  uint16_t prescaler = 0;
  uint16_t count = 0;

  uint16_t dblHz = hz * 2; // double the frequency as duty cycle uses 50% = 1 full count

  // Find the smallest prescaler.
  for (int i = 0; i < PRESCALER_COUNT; i++) {
    uint32_t c = CLOCK_SPEED / (dblHz * PRESCALERS[i]) - 1;
    if (c > 0xFFFF || c < 1)
      continue;
    prescaler = PRESCALERS[i];
    count = (uint16_t) c;
    break;
  }

  startTimer(count, prescaler);
}

// start the timer using the given 16-bit timer count and prescaler
void BigTimer2::startTimer(uint16_t count, uint16_t prescaler) {
  resetTimer();
  if (prescaler == 0 || count == 0) {
    return;
  }

  cli();

  // Split count into two bytes
  OCR2AH = (uint8_t)(count >> 8);
  OCR2A = (uint8_t)(count & 0x00FF); // COM2A Timer2 Registry

  // wave gen mode 0, all WGM22:20 zeros
  TCCR2A &= (0 << WGM21) | (0 << WGM20);
  TCCR2B &= (0 << WGM22);

  //set the output pin DDR to output
  //DDRB |= DDR_OC2A;

  // enable overflow inturrupt.
  TIMSK2 |= (1 << TOIE2);

  // Turn on the timer.
  setTimerPrescaler(prescaler);

  sei();
}


void BigTimer2::resetTimer() {
  cli();
  overflow = 0;

  // Clear control registers
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; // initialize counter value to 0
  TIMSK2 = 0; // switch off inturrupt masks.
  TIFR2  = 0; // Clear all the flags;
  sei();
}


// Called by the ISR for overflow on Timer2
void BigTimer2::Overflow() {
  overflow++;

  if (overflow == OCR2AH) {  // High byte compare match, we turn toggle the match pin state
    // Toggle current COM2A1:0 state so each match sets pin HIGH or LOW
    // B00 = no action, B01 = Toggle state, B10 = Clear state, B11 = Set state
    if (TCCR2A & (1 << COM2A0)) {
      TCCR2A &= (0 << COM2A0);
    } else {
      TCCR2A |= (1 << COM2A0);
    }
    overflow = 0;
  }

}

// Set CS22-CS20 bits in TCCR2B for PRESCALER
void BigTimer2::setTimerPrescaler(uint16_t scaler) {
  switch (scaler) {
    case 1 :
      TCCR2B |= (1 << CS20);
      break;

    case 8 :
      TCCR2B |= (1 << CS21);
      break;

    case 32 :
      TCCR2B |= (1 << CS21) | (1 << CS20);
      break;

    case 64 :
      TCCR2B |= (1 << CS22);
      break;

    case 128 :
      TCCR2B |= (1 << CS22) | (1 << CS20);
      break;

    case 256 :
      TCCR2B |= (1 << CS22) | (1 << CS21);
      break;

    case 1024 :
      TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
      break;

    default: // disable timer
      TCCR2B &= (0 << CS22);
      TCCR2B &= (0 << CS21);
      TCCR2B &= (0 << CS20);
  }
}

//ISR(TIMER2_COMPA_vect) {}

ISR(TIMER2_OVF_vect) {
  BigTimer.Overflow();
}
