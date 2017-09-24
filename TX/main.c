/*
 * main.c
 *
 * Created: 9/17/2017 1:32:11 PM
 *  Author: agus
 * MCU: attiny2313 8MHz
 */ 


#include <avr/io.h>
#include <avr/delay.h>

#define LED_PIN PB5
#define TX_PIN	PB6

int main(void)
{
	DDRD = 1<<PD2;		// Set PD2 as input (Using for interupt INT0)
	PORTD = 1<<PD2;		// Enable PD2 pull-up resistor
	DDRB = 0x00;
	
    while(1)
    {
        //ON BUTTON PRESS
		if(PIND & (1 << PD2))
		{
			//led on
			PORTB |= (1 << LED_PIN);
			
			//start transmit
			PORTB |= (1 << TX_PIN);
			_delay_ms(100);
			PORTB &= ~(1 << TX_PIN);
			_delay_ms(50);
			PORTB |= (1 << TX_PIN);
			_delay_ms(200);
			PORTB &= ~(1 << TX_PIN);			
			//end transmit
			
			//led off
			PORTB &= ~(1 << LED_PIN);
			_delay_ms(1000);
		}			
    }
}