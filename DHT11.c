#include "DHT11.h"
#include <pins_arduino.h>

static int timer_counter;

uint8_t read_dht(const uint8_t data_pin, int* data)
{
  uint8_t* buffer = malloc(5*sizeof(uint8_t));
  DDRD |= 1<<data_pin; // Set data pin as output
  TCCR0A = 0b00000010; //Timer Control Register A: Setting timer to clear on match with Output Compare Register (OCR) and generate interrupt
  TCCR0B = 0b00000010; // Setting clock scale to 1/8;
  OCR0A = 20; // Interrupt every 10us
  timer_counter = 0; // Resets Timer
  SREG |= 1<<7; // Ensures interrupts are enabled
  PORTD &= ~(1<<data_pin); // Data pin to low (Send start signal)
  TIMSK0 |= 1<<OCIE0A; // Enabling Interrupt for OCR0A
  while(timer_counter < START_SIGNAL_HIGH){__asm__("nop");} // Start signal (Why does it require a nop instruction to function?)
  PORTD |= 1<<data_pin; // Data pin to high (end start signal)
  timer_counter = 0;
  while(timer_counter < START_SIGNAL_LOW){__asm__("nop");}; // Wait for response
  PORTD &= ~(1<<data_pin); // Data pin to low (Prepare to receive DHT Response)
  DDRD &= ~(1<<data_pin); // Set data pin for input
  timer_counter = 0;
  while(!(PIND & (1<<data_pin))) // While input pin is low
  {
    __asm__("nop");
    if(timer_counter > TIMEOUT_TIME)
    {
      return 1; // Timeout
    }
  }; // Wait for response to start
  timer_counter = 0;
  while(PIND & (1<<data_pin)) // While input pin is high
  {
    __asm__("nop");
    if(timer_counter > TIMEOUT_TIME)
    {
      return 1; // Timeout
    }
  }; // Wait for data transmission to start
  for(uint8_t i = BYTE_SIZE - 1, j = 0; j < MESSAGE_SIZE; i--) // Receive 5 bytes of data
  {
    while(!(PIND & (1<<data_pin))) // While input pin is low
    {
      __asm__("nop");
      if(timer_counter > TIMEOUT_TIME)
      {
        return 1; // Timeout
      }
    }; // Wait initial low for data transmission
    timer_counter = 0;
    while(PIND & (1<<data_pin)) // While input pin is high
    {
      __asm__("nop");
      if(timer_counter > TIMEOUT_TIME)
      {
        return 1; // Timeout
      }
    }; // Measure length of high
    buffer[j] = (buffer[j] & ~(1<<i)) | (timer_counter > 5)<<i; // Puts received bit into buffer area
    if(!i)
    {
      i = BYTE_SIZE;
      j++;
    }
  }
  data[0] = buffer[2];
  data[1] = buffer[0];
  free(buffer);
  TIMSK0 &= ~(1<<OCIE0A); // Turns off timer interrupt
  return 0;
}

ISR(TIMER0_COMPA_vect)
{
  timer_counter++;
}