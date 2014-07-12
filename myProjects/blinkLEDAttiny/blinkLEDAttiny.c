/* Defines pins, ports, etc */
#include <avr/io.h>

/* Functions to waste time */
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#define ADC2 2
#define ADC3 3

// how long to give people before cutting the light (in ms)
#define DELAY 1000

// where's the relay?
#define RELAY PB0
#define SENSOR PB2

#define POT PB3
#define APOT ADC3

#define MIC PB4
#define AMIC ADC2

// how many times we saw motion in our delay cycle
volatile uint16_t motionCount;

void initInterrupt0(void) {
  // enable pin-change interrupts for bank b
  GIMSK |= (1 << INT0);

  // enable interrupt on pin PB4 (our motion sensor)
  MCUCR |= (1 << ISC00);
}

ISR(INT0_vect) {
  // on an interrupt, turn on the light immediately
  PORTB &= ~(1 << RELAY);

  // increment the motion counter
  motionCount++;

  // stop further interrupts, since we know we want it to stay on for at least
  // DELAY ms
  cli();
}

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

  // set up the interrupts
  initInterrupt0();
  initADC0();

  /* Data Direction Register B: writing a one to the bit enables output. */
  DDRB |= (1 << RELAY);
  // turn on the relay
  PORTB |= (1 << RELAY);

  // write a 0 to confirm input is enabled
  DDRB |= (0 << SENSOR);

  // set up the interrupts
  initInterrupt0();
  initADC0();

  while (1) {
    motionCount = 0;
    
    // start monitoring interrupts
    sei();

    // wait DELAY ms
    _delay_ms(DELAY);

    // stop interrupts while we process the data
    cli();

    micValue = readADC(AMIC);
    potValue = readADC(APOT);

    if (motionCount > 0) {
      // if there was motion detected, turn on the light
      PORTB &= ~(1 << RELAY);
    } else if (bit_is_set(PINB, SENSOR)) {
      // if there is currently motion, turn on the light
      PORTB &= ~(1 << RELAY);
    } else if (micValue > potValue) {
      PORTB &= ~(1 << RELAY);
    } else {
      // otherwise, turn off the light
      PORTB |= (1 << RELAY);
    }

  }

  return (0);
}
