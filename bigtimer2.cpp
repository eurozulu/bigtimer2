#include "bigtimer2.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Helper to start timer with a given frequency.
// Calculates a 16-bit counter and prescaler to achive this frequency
void BigTimer2::startTimerFrequency(uint16_t hz) {
  uint16_t prescaler = 0;
  uint16_t count = 0;

  if (hz > 0) {
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

  OCR2AH = count >> 8;
  OCR2A = count & 0x00FF;

  // wave gen mode 0, Top = Max
  TCCR2A &= (0 << WGM20);
  TCCR2A &= (0 << WGM21);
  TCCR2B &= (0 << WGM22);

  TCCR2A &= (0 << COM2A1) | (0 << COM2A0); //  Turn off compare match pins until high byte matches overflow

  //set the output pin DDR to output
  DDRB |= (1 << DDB3);

  // enable overflow inturrupt.
  TIMSK2 |= (1 << TOIE2) | (1 << OCIE2A);

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


void BigTimer2::CompareMatchA() {

  if (overflow != OCR2AH) {  // High byte compare match
    return;
  }
  
  PORTB ^= (1 << PB3);   // toggles the state of the bit
  overflow = 0;
  TCNT2 = 0;
}


// Called by the ISR for overflow on Timer2
void BigTimer2::Overflow() {
  overflow++;
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
