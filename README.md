# bigtimer2
Arduino Frequency generator using Timer2 as a pseudo 16-bit timer  
  
The need for a second 16-bit timer came from wanting to read an input frequency accurately and send out an adjusted frequency,
based on the input.  

Timer1 is the only 16-bit timer available on uno/nano atmega328p boards and the only one with an Input Capture unit.
Using Timer1 for the capture of the input frequency only leaves Timer0 and 2, both 8-bit timers.
  
The range of operation would be as low as 5hz and as high as 4-5khz.  To achieve a reasonable amount of accuracy accros this range, a 16-bit resolution was minimal.  
  
Bigtimer2 is a wrapper around Timer2 which extends its resolution to 16-bit but using the overflow interrupt to count a high byte.
Within this ISR, the overflow count is compared to a high byte OCR2A.  When matched, the control of the Timer pin OC2A (pin 11) is switched, giving a ficed 50% duty cycle.


