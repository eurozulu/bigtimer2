// big timer is an experiment to extend the resolution of Timer2 to 16-bit.
// Timer1 is the only timer which can capture timed input accurately, so requiring another 16-bit timer
// to output the 'corrected' frequency from the input from Timer1 needed at least the same resolution as the sampled frequency.
// Output is fixed to OC2A (Arduino D11) and OC2B (Arduino D3)

const int PIN_A = 11;
const int PIN_B = 3;

#include <stdint.h>

// 16mz cpu clock speed
#define CLOCK_SPEED 16000000
#define DEFAULT_PULSE_DUTY 0.4f;

#define PRESCALER_COUNT 7     // The number of prescalers available to this timer
const uint16_t PRESCALERS[PRESCALER_COUNT] {1, 8, 32, 64, 128, 256, 1024};


class BigTimer2 {
  private:
    uint8_t overflow;

    uint8_t OCR2AH; // high byte of OCR2A
    void setTimerPrescaler(uint16_t scaler);
  
  public:
    BigTimer2(){};
      
    void startTimerFrequency(uint16_t hz);
    void startTimer(uint16_t count, uint16_t prescaler);
    void resetTimer();

    void Overflow();

};

static BigTimer2 BigTimer;
