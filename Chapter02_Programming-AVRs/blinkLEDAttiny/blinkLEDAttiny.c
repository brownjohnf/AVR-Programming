/* Defines pins, ports, etc */
#include <avr/io.h>

/* Functions to waste time */
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>

// how long to give people before cutting the light (in ms)
#define DELAY 30000

// how many times we saw motion in our delay cycle
volatile uint16_t motionCount;

void initPinChangeInterrupt(void) {
  // enable pin-change interrupts for bank b
  GIFR |= (1 << PCIF);

  // enable interrupt on pin PB4 (our motion sensor)
  GIMSK |= (1 << PB4);
}

ISR(PCINT0_vect) {
  motionCount++;
  _delay_us(100);
}

int main(void) {

  // -------- Inits --------- //

  /* Data Direction Register B: writing a one to the bit enables output. */
  DDRB |= (1 << PB0);

  // turn on the relay
  PORTB |= (1 << PB0);

  // write a 0 to confirm input is enabled
  DDRB |= (0 << PB4);

  // enable the pull-up resistor on PB4
  PORTB |= (1 << PB4);

  initPinChangeInterrupt();

  // ------ Event loop ------ //
  while (1) {
    motionCount = 0;
    
    sei();
    _delay_ms(DELAY);
    cli();

    if (motionCount > 0) {
      PORTB &= ~(1 << PB0);
    } else {
      PORTB |= (1 << PB0);
    }
  }
  return (0);
}
