#include <avr/io.h>
#include <avr/interrupt.h>
#include "init.h"
#include "io_cfg.h"

void init()
{
	// *** Ustawienie trybów portów I/O ***
	
	EN_11_DIR = OUTPUT;
	EN_11 = 0;
	EN_12_DIR = OUTPUT;
	EN_12 = 0;
	EN_21_DIR = OUTPUT;
	EN_21 = 0;
	EN_22_DIR = OUTPUT;
	EN_22 = 0;
	PWM1_DIR = OUTPUT;
	PWM1 = 0;
	PWM2_DIR = OUTPUT;
	PWM2 = 0;
	
	LED_DIR = OUTPUT;
	LED = 0;
	
	BT1_DIR = INPUT;
	BT1 = 1;              // podciągnięcie wejścia do VCC
	
	BT2_DIR = INPUT;
	BT2 = 1;              // podciągnięcie wejścia do VCC
	
	
	// *** Przerwania zewnętrzne ***
	// pin int0 -> D2
	MCUCR |= (1<<ISC01); // na falling edge sygnału
    GICR |= (1<<INT0) ;       // włączenie obsługi przerwania na int0
	
	// pin int1 -> D3
	MCUCR |= (1<<ISC11); // na falling edge sygnału
    GICR |= (1<<INT1) ;       // włączenie obsługi przerwania na int1
	

	// *** TIMER 1 - PWM -  modulacja szerokości impulsu - sterowanie wypełnieniem***
	//ustawienie rejestrów zgodnie z rozdziałem 17.11 DS 
// ustawiamy:	
	//bity COM1A1/COM1B1 rejestru TCCR1A  -> dla Fast PWM w trybie nieodwracającym
	//bit WGM10 rejestru TCCR1A i bit WGM12 rejestru TCCR1B -> ustawienie Timera 1 w 8bitowy tryb Fast PWM
	//bit CS10 rejestru TCCR1B -> by wyłączyć preskalowanie licznika
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(CS10) | _BV(WGM12);
	OCR1A = OCR1B = 0;    //wyzerowanie wartości przpełnienia licznika
	

	// *** TIMER 2 - PRZERWANIE CZASOWE ***
	//ustawienie rejestrów zgodnie z rozdziałem 18.11 DS 
// ustawiamy:	
	//bit WGM21 rejestru TCCR2 -> ustawienie Timera 1 w tryb CTC - Clear Timer on Compare Match
	//bity CS22, CS21, CS20 rejestru TCCR2 -> ustawienie prescalera na 1/1024	
	
	TCCR2 |= _BV(WGM21) | _BV(CS22) | _BV(CS21) | _BV(CS00); 
	OCR2 = 78; 
	TIMSK |= _BV(OCIE2); // odblokowanie przerwań na osiągnięcie zadaniej wartości
	
	// *** ADC - analog to digital converter ***
	//ustawianie rejestrów zgodnie z rozdziałem 23.8 DS
//ustawiamy
	//bit REFS0 rejestru ADMUX -> VREF jest w module uC podłączone przez kondensator do masy
	//bit ADLAR rejestru ADMUX -> wyrównywanie 10 bitowego wyniku konwersji do lewej - otrzymujemy wartość 8 starszych bitów co daje wysarczajacą dokładność momiaru
	//bity ADPS2 i ADPS1 rejestru ADCSRA -> ustawiamy skalowanie taktowania ADC na 1/64 by uzyskać ~125kHz (zalecane 50-200kHz, taktowanie zegara uC ~8MHz)
	//bit ADEN rejestru ADCSRA -> ADC enable 
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADEN);
	ADMUX |= _BV(ADLAR) | _BV(REFS0);
	
	
	// *** dopuszczenie globalne przerwań ***
	
	sei();
}
