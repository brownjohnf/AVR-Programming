/*
A simple test of serial-port functionality.
Takes in a character at a time and sends it right back out,
 displaying the ASCII value on the LEDs.
*/

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#include "pinDefines.h"
#include "USART.h"

#define SAMPLE_DELAY  200 /* ms, controls the scroll-speed of the scope */

// -------- Functions --------- //
static inline void initFreerunningADC(void) {
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    /* ADC clock prescaler /8 */

  ADMUX |= (1 << ADLAR);     /* left-adjust result, return only 8 bits */

  ADCSRA |= (1 << ADEN);                                 /* enable ADC */
  ADCSRA |= (1 << ADATE);                       /* auto-trigger enable */
  ADCSRA |= (1 << ADSC);                     /* start first conversion */
}

int main(void) {
  clock_prescale_set(clock_div_8);
  char serialCharacter;

  // -------- Inits --------- //
  LED_DDR = 0xff;                            /* set up LEDs for output */
  initUSART();
  printString("Hello World!\r\n");                          /* to test */

  initFreerunningADC();

  // ------ Event loop ------ //
  while (1) {
    printBinaryByte(ADCH);       /* transmit the high byte, left-adjusted */
    printString("\r\n");
    _delay_ms(SAMPLE_DELAY);


    // serialCharacter = receiveByte();
    // transmitByte(serialCharacter);
    // printString(", ");
    // printBinaryByte(serialCharacter);
    // printString("\r\n");
    // LED_PORT = serialCharacter;
                           /* display ascii/numeric value of character */

  }                                                  /* End event loop */
  return (0);
}
