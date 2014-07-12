// /*
//   Demo of using interrupts for doing what they do best --
//   two things at once.

//   Flashes LED at a fixed rate, interrupting whenever button is pressed.
// */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// // sensor port / pin
#define BUTTON_DDR      PORTB
#define BUTTON_PORT     PORTB
#define BUTTON_PIN      PINB

// // light/fan port / data register
#define LED_PORT        PORTB
#define LED_DDR         DDRB

// // // IR sensor input, high when motion is detected
#define BUTTON          PB3

// // led indicating operation (nightlight?)
#define LED_RED         PB0
#define LED_GREEN       PB2

#define DEBOUNCE_TIME   1000

volatile uint8_t timerRun;
volatile uint8_t timerReady;

// how long to delay before turning off (4 units / second)
#define TIMEOUT           255

void initInterrupt0(void) {

  // General Interruput Mask Register
  // Pin Change Interrupt Enable
  GIMSK |= (1 << PCIE);

  // Pin Change Mask Register
  PCMSK = (1 << PB3);

  sei();
}

uint8_t debounce(void) {
  if (bit_is_clear(BUTTON_PIN, BUTTON)) {
    _delay_ms(DEBOUNCE_TIME);

    if (bit_is_clear(BUTTON_PIN, BUTTON)) {
      return (1);
    }
  }

  return (0);
}

void stopTimer(void) {
  LED_PORT &= ~(1 << LED_RED);
  LED_PORT |= (1 << LED_GREEN);
  timerRun = 1;
  timerReady = 0;
}

void startTimer(void) {
  LED_PORT |= (1 << LED_RED);
  LED_PORT &= ~(1 << LED_GREEN);
  timerRun = 1;
  timerReady = 1;
}

void shutOff(void) {
  LED_PORT &= ~(1 << LED_RED) & ~(1 << LED_GREEN);
  timerReady = 0;
  timerRun = 0;
}

void turnOn(void) {
  LED_PORT |= (1 << LED_RED);
  LED_PORT &= ~(1 << LED_GREEN);
  timerReady = 1;
  timerRun = 0;
}

ISR(PCINT0_vect) {
  // if pin is low
  if (debounce()) {
    if (timerRun == 0) {
      if (timerReady == 0) {
        turnOn();
      } else {
        startTimer();
      }
    } else {
      shutOff();
    }
  }
}

void initTimer0(void) {
  // TCCR1 |= (1 << CS02) | (1 << CS00);
  // TCCR1 |= 1<<COM1A0
}

int main(void) {
  LED_DDR |= (1 << LED_RED) | (1 << LED_GREEN);
  BUTTON_PORT |= (1 << BUTTON);

  initInterrupt0();
  initTimer0();

  shutOff();

  uint8_t timerValue;
  uint16_t counter = 455;
  uint16_t max = 455;

  while (1) {
    // read the timer, dropping the 8 least sig bits (dividing by 256)
    // timerValue = TCNT1;


    if (timerRun == 1) {
      if (timerReady == 1) {
        LED_PORT ^= (1 << LED_RED);
        counter--;

        if (counter > max >> 1) {
          _delay_ms(8000);
        } else if (counter > max >> 2) {
          _delay_ms(4000);
        } else if (counter > max >> 3) {
          _delay_ms(2000);
        } else if (counter > 0) {
          _delay_ms(1000);
        } else {
          stopTimer();
          counter = 455;
        }
      } else {
        LED_PORT ^= (1 << LED_GREEN);
        _delay_ms(1000);
      }
    }

    // if we're over the timeout and the sensor's off, turn off the light
    // if (timerValue > TIMEOUT) {
    //   if (bit_is_clear(SENSOR_PIN, SENSOR)) {
    //     stopTimer();
    //   }
    // }
  }
}
