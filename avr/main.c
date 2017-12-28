#include <avr/io.h>
#include <util/delay.h>

#define DELAY_MS 100
 
int main()
{
  DDRD = 0x80;  // Configure pin D0 as output
 
  while(1) {
    PORTD = 0xFF; // Turn on LED
    _delay_ms(DELAY_MS);

    PORTD = 0x00; // Turn off LED
    _delay_ms(DELAY_MS);
  }

  return 0;
}
