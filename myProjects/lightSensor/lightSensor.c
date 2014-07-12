// Quick and dirty adjustable-threshold night-light.

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>

#define ADC2 2
#define ADC3 3

// where's the relay?
#define RELAY PB0
#define SENSOR PB2

#define POT PB3
#define APOT ADC3

#define MIC PB4
#define AMIC ADC2


// -------- Functions --------- //
static inline void initADC0(void) {
  // Analog Comparator Multiplexer Enable bit
  ADCSRA |= (1 << ACME);

  ADCSRA |= (1 << ADPS2);                   /* ADC clock prescaler /16 */
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */
}

uint16_t readADC(uint8_t channel) {
  // select ADC2
  ADMUX = (0xf0 & ADMUX) | channel;

  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);

  return(ADC);
}

int main(void) {
  uint16_t micValue;
  uint16_t potValue;

  // slow down the clock to 1MHz
  clock_prescale_set(clock_div_8);

  initADC0();

  // set relay to output
  DDRB |= (1 << RELAY);
  // turn on the led
  PORTB |= (1 << RELAY);


  // ------ Event loop ------ //
  while (1) {
    micValue = readADC(AMIC);
    potValue = readADC(APOT);

    if (micValue > potValue) {
      PORTB &= ~(1 << RELAY);
      _delay_ms(100);
    } else {
      PORTB |= (1 << RELAY);
    }

  }                                                  /* End event loop */
  return (0);                            /* This line is never reached */
}
