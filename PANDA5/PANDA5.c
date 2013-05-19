/*
 * PANDA5.c
 *
 * Created: 2013-05-19 18:14:48
 *  Author: Haros
 */ 

#include "io_cfg.h"
#include "init.h"
#include "linefollower.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	init();
	
	LED = 1;
	
	while(1)
	{
		if(exe_loop && on)
		{
			exe_loop = 0;
			loop_LF();
		}
	}
}

ISR(TIMER2_COMP_vect)			// obsługa przerwania od Timera 2
{
	exe_loop = 1;
}

ISR(INT0_vect)				// obsługa przerwania od przycisku/switcha
{
	GICR &= ~(1<<INT0) ;        // wyłączamy obsługę przerwania na int0 ->ew. drgania styków mogłby powodować wielokrotne wywołanie
	
	_delay_ms(50);				// czekamy na ustanie drgań styków
	
	on	 ^= 1;					// zmieniamy stan flagi
	
	GICR |= (1<<INT0) ;		// włączamy z powrotem obsługę przerwania
}
