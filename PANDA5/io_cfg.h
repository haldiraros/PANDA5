#ifndef IO_CFG_H
#define IO_CFG_H

// konfiguracja wyjść I/O

#define F_CPU 8000000UL

#define INPUT 0
#define OUTPUT 1

typedef struct  //struktura rejestrów portów
{
  unsigned int bit0:1;
  unsigned int bit1:1;
  unsigned int bit2:1;
  unsigned int bit3:1;
  unsigned int bit4:1;
  unsigned int bit5:1;
  unsigned int bit6:1;
  unsigned int bit7:1;
} _io_reg; 

#define REGISTER_BIT(rg,bt) ((volatile _io_reg*)&rg)->bit##bt   //makro do konfiguracji wyjść rg- który rejstr portu, który rejestr kierunku


/* teraz następuje ustawienie makr do obsługi odpowiednich wyjść/wejść układu - nazwy rejestrów wyczytane z datasheeta uC 
	PORTx - ustawienie wartości pinu
	DDRx - kierunek portu/pinu
	PINx - informacja o stanie pinu wejściowego
	
*/

#define EN_11				REGISTER_BIT(PORTD,4) //pierwsze wyjście kierunkowe pierwszego silnika
#define EN_11_DIR			REGISTER_BIT(DDRD,4)
#define EN_12				REGISTER_BIT(PORTD,5) //drugiego wyjście kierunkowe pierwszego silnika
#define EN_12_DIR			REGISTER_BIT(DDRD,5)
#define EN_21				REGISTER_BIT(PORTD,6) //pierwsze wyjście kierunkowe drugiego silnika
#define EN_21_DIR			REGISTER_BIT(DDRD,6)
#define EN_22				REGISTER_BIT(PORTD,7) //drugie wyjście kierunkowe drugiego silnika
#define EN_22_DIR			REGISTER_BIT(DDRD,7)

#define PWM1				REGISTER_BIT(PORTB,1) // wyjście z sygnałem PWM (wypełnienie prądowe) dla pierwszego silnika
#define PWM1_DIR			REGISTER_BIT(DDRB,1)
#define PWM2				REGISTER_BIT(PORTB,2) // wyjście z sygnałem PWM dla drugiego silnika
#define PWM2_DIR			REGISTER_BIT(DDRB,2)
#define PWMP				OCR1B				  // etykiety dla odpowiednich rejestrów Timera do sterowania PWM
#define PWML				OCR1A					

#define LED					REGISTER_BIT(PORTB,0)  //wyjście dla diody LED
#define LED_DIR				REGISTER_BIT(DDRB,0)

#define BT1					REGISTER_BIT(PORTD,3) //wejście dla guzika 1 - dopalanie LF
#define BT1_DIR				REGISTER_BIT(DDRD,3)
#define BT1_PIN				REGISTER_BIT(PIND,3)  //rejestr informujący o stanie pinu

#define BT2					REGISTER_BIT(PORTD,2) //wejście dla guzika 2 - odpalenie konfiguracji
#define BT2_DIR				REGISTER_BIT(DDRD,2)
#define BT2_PIN				REGISTER_BIT(PIND,2)  //rejestr informujący o stanie pinu

#endif
