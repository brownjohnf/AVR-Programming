// Quick and dirty adjustable-threshold night-light.

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"

/* milliseconds */
#define DELAYTIME 100

uint16_t readADC(uint8_t channel) {
  ADMUX = (0xf0 & ADMUX) | channel;
  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADC);
}

int main(void) {
  // -------- Inits --------- //
  uint16_t lightThreshold;
  uint16_t sensorValue;

  // for iterating through the eyes
  uint8_t i;

  // -------- Inits --------- //
  /* Data Direction Register B: all set up for output */
  LED_DDR = 0xff;   

  // Set up ADC
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0);     /* ADC clock prescaler /8 */
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */

  // ------ Event loop ------ //
  while (1) {

    lightThreshold = readADC(POT);
    sensorValue = readADC(LIGHT_SENSOR);

    if (sensorValue < lightThreshold) {

      while (i < 7) {
        LED_PORT = (1 << i);                 /* illuminate only i'th pin */
        _delay_ms(DELAYTIME);                                    /* wait */
        i = i + 1;                               /* move to the next LED */
      }

      while (i > 0) {
        LED_PORT = (1 << i);                 /* illuminate only i'th pin */
        _delay_ms(DELAYTIME);                                    /* wait */
        i = i - 1;                           /* move to the previous LED */
      }
    }
    else {
      LED_PORT = 0x00;
    }
  }                                                  /* End event loop */
  return (0);                            /* This line is never reached */
}
