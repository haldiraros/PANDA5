#include <avr/io.h>
#include <stdlib.h>
#include "io_cfg.h"
#include "linefollower.h"

int prog = 0;     // próg przy którym odczytana z ADC wartość uznwana jest za =1
int array_sensors[7] = {5,4,3,2,1,0,7}; // tablica podająca do którego portu ADC jest podłączony dany sensor
int sensors[7] = {0}; 	//tablica w której zapamiętywane jest czy sensor wykrył linię
	
int prev_err = 0, overshot = 0;  // błąd w porzednim przebiegu, flaga "przestrzelenia" zakrętu
int blad = 0, pop_blad = 0, Kp = 1, Kd = 0; //zmienne dla regulatora PD
int V_zad = 100;  //nominalna wartość wypełnienia sygnału sterującego silnikiem

volatile int on = 0, exe_loop = 0;  //flagi

void PWM(int left, int right)  //funkcja ustawiająca odpowiednie wypełnienie dla Timera 1 i kierunek jazdy
{
	if(left >= 0) //jeżeli lewy do przodu
	{
		if(left>255) //zabezpieczenie przed przekroczeniem maksymalnej wartości 
			left = 255;
		EN_12 = 1;  //ustawienie odpowiedniego kierunku prądu na mostku H
		EN_11 = 0;
	}
	else
	{
		if(left<-255)
			left = -255;
		EN_12 = 0;
		EN_11 = 1;	
	}
	
	if(right >= 0)
	{
		if(right>255)
			right = 255;
		EN_22 = 1;
		EN_21 = 0;
	}
	else
	{
		if(right<-255)
			right = -255;
		EN_22 = 0;
		EN_21 = 1;	
	}
	
	PWML = abs(left); // ustawienie wypełnienia sygnału dla lewego silnika
	PWMP = abs(right); // ustawienie wypełnienia sygnału dla prawego silnika
}


void calibrate() // funkcja kalibrująca sensory - ustawienie progu na podstawie wartosci wykrywanych przez dwa lewe czujniki
{
    int white, black;
    //odczyt z pierwszego czujnika - nad białym polem
    ADMUX &= 0b11100000;				// zerujemy bity MUX odpowiedzialne za wybór kanału (rozdział 23.8.1 w DS)
    ADMUX |= 1;							// wybieramy kanał przetwonika ADC z którego chcemy korzystać
    ADCSRA |= _BV(ADSC);				// uruchamiamy pomiar
    while(ADCSRA & _BV(ADSC)) {};		// czekamy do końca pomiaru (bit przestawi się na 0)
    white = ADCH;						// odczyt z czujnika nad białym polem
    
	//odczyt z drugiego czujnika - nad czarną linią, podobnie jak poprzednio
    ADMUX &= 0b11100000;
    ADMUX |= 2;
    ADCSRA |= _BV(ADSC);
    while(ADCSRA & _BV(ADSC)) {};
    black = ADCH;						// odczyt z czujnika nad linią
    
    prog = white + (black-white)/2;	    // ustalenie progu pośrodku zmierzonych wartości
}


void read_adc() //funkcja służąca do sprawdzenia wszystkich czujników czy znajdują się nad linią
{
    for(int i=0; i<7; i++)
    {
        ADMUX &= 0b11100000;			// zerujemy bity MUX odpowiedzialne za wybór kanału (rozdział 23.8.1 w DS)
        ADMUX |= array_sensors[i];		// wybieramy kanał przetwonika ADC z którego chcemy korzystać
        ADCSRA |= _BV(ADSC);			// uruchamiamy pomiar
        while(ADCSRA & _BV(ADSC)) {};	// czekamy do końca pomiaru (bit przestawi się na 0)
        
        if(ADCH > prog)					// porównanie stanu 8 starszych bitów z progiem, stwierdzenie czy czujnik jest nad linią
            sensors[i] = 1;
        else
            sensors[i] = 0;
    }
}


int calc_err()
{
	int err = 0;						// wartość błędu
	int count = 0;						// ilość czujników które wykryły linię
	
	int weight = 10;						// współczynnik wagi czujników, domyślnie 10
	
	if(overshot) weight = 5;				// zmniejszenie wag czujników w przypadku przestrzelenia zakrętu, pozwala to ograniczyć wachania po powrocie na linię
	
	for(int i=0; i<7; i++)
	{
		err += sensors[i]*(i-3)*weight;	// sensor [i] ma wartość  1/0; waga czujnika zależna od pozycji
		count += sensors[i];            // odliczenie stanu sensora do liczby sensorów nad linią
	}
	
	if(count != 0)						// jeżeli jakikolwiek czujnik wykrył linię
	{
		err /= count;					// obliczamy aktualny błąd
		prev_err = err;
	}
	else								//  żaden czujnik nie wykrył linii
	{
		if(prev_err < -20)				// ostatnio wykryta linia była po lewej stronie
		{
			err = -40;					// ustawiamy błąd na większy od błędu ostatniego lewego czujnika - szybszy powrót na linię
			overshot = 1;			// ustawienie flagi przestrzelenia, 1 -> linia po lewej
		}
		else if(prev_err > 20)			// ostatnio wykryta linia była po prawej stronie
		{
			err = 40;					// ustawiamy błąd na większy od błędu ostatniego prawego czujnika
			overshot = 2;			// ustawienie flagi przestrzelenia, 2 -> linia po prawej
		}
		else
			err = 0;
	}
	
	if(overshot == 1 && err >= 0)	// zerowanie flagi przestrzelenia zakrętu po powrocie na środek linii
	overshot = 0;
	else if(overshot == 2 && err <= 0)
	overshot = 0;
	
	return err;
}


int PD() //regulator PD
{
	int rozniczka = blad - pop_blad;
	pop_blad = blad;
	
	return Kp*blad + Kd*rozniczka;
}


void loop_LF() //główna pętla sterująca
{
	read_adc(); //odczyt sensorów
	blad = calc_err(); //obliczenie błędu
	int regulacja = PD(); //obliczenie wartości regulacji
	PWM(V_zad + regulacja, V_zad - regulacja); //ustawienie odpowiedniego wypełnienia sygnału dla silników
}
