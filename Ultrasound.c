#include "Ultrasound.h"
#include <pins_arduino.h>

static int timer_counter;

double get_distance(uint8_t trigger_pin, uint8_t echo_pin)
{
  if(trigger_pin > 7 || echo_pin > 7 || trigger_pin == echo_pin)
  {
    return 0;
  }
  DDRD |= 1<<trigger_pin;
  DDRD &= !(1<<echo_pin);
  TCCR0A = 0b00000010; //Timer Control Register A: Setting timer to clear on match with Output Compare Register (OCR) and generate interrupt
  TCCR0B = 0b00000010; // Setting clock scale to 1/8;
  OCR0A = 20; // Interrupt every 10us
  PORTD |= 1<<trigger_pin; // Turns on trigger pin
  TIMSK0 |= 1<<OCIE0A; // Enabling Interrupt for OCR0A
  timer_counter = 0;
  while(timer_counter != 0){}; // Sends signal for 10us
  PORTD &= !(1<<trigger_pin); // Tunrs off trigger pin
  while(!(PIND & (1<<echo_pin))){};
  timer_counter = 0;
  while(PIND & (1<<echo_pin)){};
  TIMSK0 &= !(1<<OCIE0A); // Turns off OCR0A Interrupt
  return (10.0*timer_counter)/58;
}

ISR(TIMER0_COMPA_vect)
{
  timer_counter++;
}