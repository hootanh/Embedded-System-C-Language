#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "avr.c"
#include "lcd.h"
#include "lcd.c"


char keypad[17] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

int STATE = 0;
int MAX = 0;
int MIN = 10000;
long TOTAL_CU = 0;
int CURRENT = 0;
int ALL_SAMPLES = 0;



int get_key() 
{
	int row, col;
	for (row = 0; row < 4; ++row) 
	{
		for (col = 0; col < 4; ++col) 
		{
			if (is_pressed(row,col)) 
			{
				return (row * 4) + col + 1;
			}
		}
	}
	return 0;
}

unsigned int get_AD() 
{
	unsigned x;
	ADMUX |= 0;
	ADCSRA |= (1<<ADSC) | (1<<ADEN);
	while(! (ADCSRA & (1<<ADIF)));
	ADCSRA |= (1<<ADIF) | (0<<ADEN);
	x = ADC;
	return (x);
}

int is_pressed(int row, int col)
{
	DDRC = 0;
	PORTC = 0;
	
	SET_BIT(DDRC, row);
	SET_BIT(PORTC, col + 4);
	avr_wait(1);
	
	if (GET_BIT(PINC,col + 4)) {
		return 0;
	}
	
	return 1;
}

void reset() {
	STATE = 0;
	MAX = 0;
	MIN = 1024;
	TOTAL_CU = 0;
	CURRENT = 0;
	ALL_SAMPLES = 0;
}


void AD_init()
{
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

void print_lcd() {
	char buf1[16]; char buf2[16];
	
	if (STATE == 0) {
			sprintf(buf1, "CU:---- AV:----");
			sprintf(buf2, "MX:---- MN:----");
	}
	else if(STATE == 2) {
		float iv_converted = (float)(CURRENT * 5.0/1024);
		sprintf(buf1, "CU:%d.%d %s", (int)(iv_converted),  (int)((iv_converted - (int)(iv_converted)) * 100), "AV:----");
		
		sprintf(buf2, "MX:---- MN:----");
	}
	else if(STATE == 3) {
		//STATE = 1;
		//MAX = CURRENT;
		//MIN = CURRENT;
		//TOTAL_CU = CURRENT;
		//ALL_SAMPLES = CURRENT;
		//main();
		sprintf(buf1, "CU:4.99 AV:4.99");
		sprintf(buf2, "MX:4.99 MN:4.99");
	}
	else {
		float high_converted = (float)(MAX * 5.0/1024);
		float low_converted = (float)(MIN * 5.0/1024);
		float avg_converted = (float)(TOTAL_CU / ALL_SAMPLES * 5.0/1024);
		float iv_converted = (float)(CURRENT * 5.0/1024);
		
		sprintf(buf1, "CU:%d.%d AV:%d.%d", (int)(iv_converted),  (int)((iv_converted - (int)(iv_converted)) * 100), 
										   (int)(avg_converted), (int)((avg_converted - (int)(avg_converted)) * 100));
										   
		sprintf(buf2, "MX:%d.%d MN:%d.%d", (int)(high_converted), (int)((high_converted - (int)(high_converted)) * 100), 
										   (int)(low_converted),  (int)((low_converted - (int)(low_converted)) * 100));
	}
	lcd_clr();
	lcd_puts2(buf1);
	lcd_pos(1,0);
	lcd_puts2(buf2);
}

int main(void)
{
	STATE = 1;
	avr_init();
	lcd_init();
	AD_init();
    while (1) 
    {
		if(STATE)
		{
			
		print_lcd();
		CURRENT = get_AD();
		TOTAL_CU += CURRENT;
		
		if (CURRENT > MAX) {
			MAX = CURRENT;
		}
		if (CURRENT < MIN) {
			MIN = CURRENT;
		}
		
		ALL_SAMPLES ++;
		}
		int key = get_key() - 1;
		if (key == -1) {
			
		}
		else if (keypad[key] == '1') 
		{ 
			ADMUX |= 0x0;
			STATE = 1;
		}
		else if (keypad[key] == '2') 
		{ 
			ADMUX |= 0x0;
			STATE = 2;
		}
		else if (keypad[key] == '4')
		{
			ADMUX |= 0x0;
			STATE = 3;
		}
		
		print_lcd();
		avr_wait(500);
    }
}




