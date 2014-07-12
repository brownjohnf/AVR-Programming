/*
  Demo of using interrupts for doing what they do best --
  two things at once.

  Flashes LED at a fixed rate, interrupting whenever button is pressed.
*/

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// sensor port / pin
#define SENSOR_PORT       PORTD
#define SENSOR_PIN        PIND

// light/fan port / data register
#define LIGHT_PORT        PORTB
#define LIGHT_DDR         DDRB

// IR sensor input, high when motion is detected
#define SENSOR            PD2

// output to relay for light; high signal activates rela
#define LIGHT             PB1

// led indicating operation (nightlight?)
#define LED               PB0

// how long to delay before turning off (4 units / second)
#define TIMEOUT           124

static inline void initTimer(void) {
  // Normal mode (default), just counting
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

void turnLightOn(void) {
  LIGHT_PORT |= (1 << LIGHT);
  LIGHT_PORT &= ~(1 << LED);
}

void turnLightOff(void) {
  LIGHT_PORT &= ~(1 << LIGHT);
  LIGHT_PORT |= (1 << LED);
}

// Run every time there is a change on sensor line
ISR(INT0_vect) {
  turnLightOn();

  // zero-out the timer
  TCNT1 = 0;
}

void initInterrupt0(void) {
  // enable INT0
  EIMSK |= (1 << INT0);
  // trigger when button changes
  EICRA |= (1 << ISC00);
  // set (global) interrupt enable bit
  sei();
}

int main(void) {
  // active the light port
  LIGHT_DDR = 0xff;

  // turn on the pull-up resistor on the sensor
  SENSOR_PORT |= (1 << SENSOR);

  // start the interrupts
  initInterrupt0();

  // start the timer
  initTimer();

  // define vars
  uint16_t timerValue;

  // start with the light on on a reset
  turnLightOn();

  while (1) {

    // read the timer, dropping the 8 least sig bits (dividing by 256)
    timerValue = TCNT1 >> 8;


    // if we're over the timeout and the sensor's off, turn off the light
    if (timerValue > TIMEOUT) {
      if (bit_is_clear(SENSOR_PIN, SENSOR)) {
        turnLightOff();
      }
    }
  }

  return (0);
}
