#include <avr/io.h>
#include "avr.h"
#include "lcd.c"


int dd, mm, yy, h, min, sec, mil, civ, am, pm;
mil = 1;
civ = 0;
am = 0;
pm = 0;

void display_time(int row, int mon, int day, int year)
{

	char buf[17];
	lcd_pos(row,1);
	sprintf(buf,"%02i/%02i/%04i", mon,day,year);
	lcd_puts(buf);
}

void display_date(int r,int hour, int min, int sec) 
{
	char buf[17];
	lcd_pos(r,1);
	sprintf(buf,"%02i:%02i:%02i", hour,min,sec);
	lcd_puts(buf);
}

void time_counter()
{
		if(++sec == 60) {
			sec = 0;
			++min;
			if(min == 60) {
				min = 0;
				++h;
				if(mil) {
					if(h == 12 ){
						pm = 1;
						am = 0;
					}
					if(h == 24){
						h = 0;
						pm = 0;
						am = 1;
						date_counter();
					}
				}
				else if(h == 12){
					h = 12;
					date_counter();
					pm = 0;
					am = 1;
				}
			}
		}
}

void date_counter()
{
	if(!is_valid_date(mm , ++dd, yy)){
		dd = 1;
		++mm;
		if(!is_valid_date(mm, dd, yy)){
			mm = 1;
			++yy;
		}
	}
}

void setup()
{
	mil = 1;
	civ = 0;
	lcd_clr();
	avr_wait(500);
	int k = get_key();
	lcd_clr();
	do
	{
		display(0, "Set Time -> B");
		display(1, "Set Date -> C");
		avr_wait(500);
		k = get_key();	
	
	
	if(k == 8)
	{
		lcd_clr();
		set_t();
		lcd_clr();
		display(0, "Time Set");
		avr_wait(3000);
		
	}
	if(k == 12)
	{
			lcd_clr();
			set_d();
			lcd_clr();
			display(0, "Date Set");
			avr_wait(3000);

	}
	}while(k!=16);
	lcd_clr();
	
}

void set_d()
{
	char buf[17];
	int key = get_key();
	int num = 0;
	int loop = 0;
	int temp[8] = {0,0,0,0,0,0,0,0};
	do {
		while(loop < 8)
		{
			lcd_pos(0, loop+1);
			key = get_key();
			if(is_number(key))
			{
				num = key_to_num(key);
				sprintf(buf, "%i", num);
				lcd_puts(buf);
				avr_wait(300);
				temp[loop] = num;
				loop++;
			}
		}
		mm = temp[0] * 10 + temp[1];
		dd = temp[2] * 10 + temp[3];
		yy = temp[4] * 1000 + temp[5] * 100 + temp[6] * 10 + temp[7];

		if (!is_valid_date(mm, dd, yy))
		{
			lcd_clr();
			display(0, "Invalid Date!!!!");
			avr_wait(1500);
			lcd_clr();
			loop = 0;
			mil = 1;
			civ = 0;
			am = 0;
			pm = 0;
			set_d();
		}

	}while(!is_valid_date(mm, dd, yy));
}

void set_t()
{
	char buf[17];

	int key = get_key();
	int num = 0;

	int loop = 0;
	int temp[6] = {0,0,0,0,0,0};
	do{
		while(loop < 6) {
			lcd_pos(1,loop+1);
			key = get_key();
			if(is_number(key)) {
				num = key_to_num(key);
				sprintf(buf,"%i",num);
				lcd_puts(buf);
				avr_wait(300);
				temp[loop] = num;
				loop ++;
			}
			
		}
		h = temp[0] * 10 + temp[1];
		min = temp[2] * 10 + temp[3];
		sec = temp[4] * 10 + temp[5];

		if (!is_valid_time(h,min,sec))
		{
			lcd_clr();
			display(0, "Invalid time!!!!");
			avr_wait(1500);
			lcd_clr();
			loop = 0;
			mil = 1;
			civ = 0;
			am = 0;
			pm = 0;
			set_t();
		}
	}while(!(is_valid_time(h,min,sec)));
}

int is_valid_time(int hour,int minute,int second)
{
	if (second < 60) {
		if ( minute < 60) {
			if (mil) {
				if(hour < 24)
				return 1;
				} else if(civ) {
				if(hour < 12)
				return 1;
			}
		}
	}
	return 0;
}

int is_valid_date(int month, int day, int year)
{
	int month_year[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if( month == 2 && day == 29 && !is_leap_year(year))
	{
		return 0;
	}
	if( month == 2 && day == 29 && is_leap_year(year))
	{
		return 1;
	}
	if (month >= 0 && month <= 12)
	{
		if (day > 0 && day <= month_year[month])
		{
			return 1;
		}
	}
	return 0;
}

int is_leap_year(int year){
	if((year % 400 == 0) || (year % 4 == 0 && year % 100 !=0)) {
		return 1;
	} else return 0;
}

int key_to_num(int key)
{
	if(key < 4)
	{
		return key;
	}
	else if(key < 8)
	{
		return (key-1);
	}
	else if(key < 12)
	{
		return (key-2);
	}
	else{
		return 0;
	}
}

int is_number(int key)
{
	int key_pad [10] = {1,2,3,5,6,7,9,10,11,14};
	int i;
	for(i= 0; i < 10; ++i)
	{
		if(key == key_pad[i])
		{
			return 1;
		}
	}
	return 0;
}

void display(int r, char * str)
{
	char buffer[17];
	lcd_pos(r, 0);
	sprintf(buffer, "%s", str);
	lcd_puts(buffer);
}

int is_pressed(int row, int col)
{
	DDRC = 0;
	PORTC = 0;
	SET_BIT(PORTC,row);
	SET_BIT(DDRC,col + 4);
	CLR_BIT(PORTC,row);
	if (GET_BIT(PINC,row))
	{
		return 0;
	}
	return 1;
}

int get_key(void)
{
	int row,col;
	for(row = 0; row < 4; ++row)
	{
		for(col = 0; col < 4; ++col)
		{
			if (is_pressed(row,col))
			return 1 + (row*4) + col;
		}
	}
	return -1;
}

void avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void avr_init(void)
{
	WDTCR = 15;
}

int main(void)
{
	lcd_init();
	lcd_clr();
	setup();
	int k = get_key();
	while (1)
	{
		avr_wait(800);
		time_counter();
		display_time(0, mm, dd, yy);
		display_date(1, h, min, sec);
		k = get_key();
		avr_wait(10);
		if(k == 4)
		{
			setup();
		}
		
	}
	return 1;
}