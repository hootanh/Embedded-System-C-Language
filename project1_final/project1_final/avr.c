#include "avr.h"
#include <avr/io.h>
#include <util/delay.h>

void avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

int main(void)
{
	SET_BIT(DDRB, 0);
	CLR_BIT(DDRB, 1);
	
	while(1) {
		if(GET_BIT(PINB, 1) == 0) 
		{
			SET_BIT(PORTB, 0);
			avr_wait(500); 
			CLR_BIT(PORTB, 0);
			avr_wait(500);
		}
	}
	return 0;
}
