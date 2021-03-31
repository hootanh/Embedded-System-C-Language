#include "avr.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short subsec)
{
	TCCR0 = 3;
	while (subsec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64));
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}
void
avr_wait2(unsigned short subsec)
{
	TCCR0 = 3;
	while (subsec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.00005);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}