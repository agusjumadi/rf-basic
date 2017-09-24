/*
 * GccApplication2.cpp
 *
 * Created: 9/17/2017 9:23:31 AM
 *  Author: agus
 */ 
/*
 * rf_receiver.c
 *
 * Created: 05.08.2017 21:42:04
 * Author : Agus
 * MCU	: atmega328 16MHz
 */


#ifndef F_CPU
#define F_CPU 8000000UL                    // set the CPU clock
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include <util/delay.h>
#include "uart.h"

const unsigned int UPPER_THRESHOLD = 500;  //upper threshold value
const unsigned int LOWER_THRESHOLD = 80;  //lower threshold value

#define UART_BAUD_RATE      9600  
 
void InitADC()
{
	ADMUX=(1<<REFS0);                         // For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Rrescalar div factor =128
}

uint16_t ReadADC(uint8_t ch)
{
   //Select ADC Channel ch must be 0-7
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);

   return(ADC);
}

void Wait()
{
   uint8_t i;
   for(i=0;i<20;i++)
      _delay_loop_2(0);
}

ISR(TIMER1_OVF_vect)
{
	//TCNT1=0xD5CF;
	PORTB|=(1<<PB5);
	_delay_ms(2000);
	PORTB&=~(1<<PB5);
}

/**
keterangan 
- = 50ms (1x)
_ = 100ms (2x)
__ = 200ms (4x)

----_-_- => 00
----_-__- => 01
----__-_- => 10
----__-__- => 11
*/
int main(void)
{
    DDRB |= (1 << PB5) ;   //LED

	uint16_t adc_result, last_val, toleransi;
	char buffer[7];
	int storedData[3];
	last_val = 0;
	toleransi = 5;
	int lenFreq;
	int maxLen = 100; // jika melebihi 200ms 
	
	//Initialize ADC
	InitADC();
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    // lets turn on 16 bit timer1 also with /1024
	TCCR1B |= (1 << CS10) | (1 << CS12);
	//Enable Overflow Interrupt Enable
	TIMSK1 |=(1<<TOIE1);
	TCNT1=0xD5CF; // timer 1 second
    sei();
	
	uart_puts("Selamat datang\r\n");
	
	
    while (1)
    {
		lenFreq = 0;
		adc_result=ReadADC(0);           // Read Analog value from channel-0
		if(adc_result > UPPER_THRESHOLD) continue;
		
		while(adc_result < LOWER_THRESHOLD && lenFreq <= maxLen)
		{
			lenFreq++;
		}
		if(lenFreq >= maxLen)continue;
		
		storedData[0] = lenFreq;
		
		while(adc_result > UPPER_THRESHOLD && lenFreq <= maxLen)
		{
			lenFreq++;
		}
		if(lenFreq >= maxLen)continue;
		storedData[1] = lenFreq;
		
		while(adc_result < LOWER_THRESHOLD && lenFreq <= maxLen)
		{
			lenFreq++;
		}
		if(lenFreq >= maxLen)continue;
		storedData[2] = lenFreq;
    }
}

