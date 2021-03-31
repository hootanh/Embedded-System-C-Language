#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include "avr.c"
#include "lcd.c"


int song_name = 0;

struct Note  
{
	int frequency;
	int duration;
};

struct Note song1[42] = { {3,1}, {3,1}, {10,1}, {10,1}, {0,1}, {0,1}, {10,0}, {8,1}, {8,1}, {7,1}, {7,1}, {5,1}, {5,1}, {3,0},
{10,1}, {10,1}, {8,1}, {8,1}, {7,1}, {7,1}, {5,0}, {10,1}, {10,1}, {8,1}, {8,1}, {7,1}, {7,1}, {5,0},
{3,1}, {3,1}, {10,1}, {10,1}, {0,1}, {0,1}, {10,0}, {8,1}, {8,1}, {7,1}, {7,1}, {5,1}, {5,1}, {3,0} };
struct Note song2[13] = {{7,1}, {5,1}, {3,1}, {5,1}, {7,1}, {7,1}, {7,0}, {5,1}, {5,1}, {5,0}, {7,1}, {10,1}, {10,0}};

#define TOTAL_NOTES 42
int FREQUENCY[TOTAL_NOTES] = {220, 233, 246, 261, 277, 293, 311, 329, 349, 369, 391, 415};
int DURATION_MODIFIER[3] = {1,2,4};
int DURATION = 200;
int melody = 0;
float TH_TL_MODIFIER = 1.0;

char keypad[17] = {'1', '2', '3', 'A','4', '5', '6', 'B','7', '8', '9', 'C','*', '0', '#', 'D'};

void play_note(int frequency,int duration)
{
	int total_duration = ceil((20000.0 / frequency));
	int TH = (total_duration / 2) * TH_TL_MODIFIER;
	
	if(TH == total_duration)
	{
		TH -= 1;
	}
	int TL = total_duration - TH;
	
	lcd_clr();
	char buf[17]; 
	char buf2[17];
	
	if (song_name == 1)
	{
		lcd_clr();
		lcd_pos(0,0);
		sprintf(buf, "%s", "Song Name:");
		lcd_puts(buf);
		lcd_pos(1,1);
		sprintf(buf2, "%s", "==> Twinkle <==");
		lcd_puts(buf2);
	}
	if (song_name == 2)
	{
		lcd_clr();
		lcd_pos(0,0);
		sprintf(buf, "%s", "Song Name:");
		lcd_puts(buf);
		lcd_pos(1,2);
		sprintf(buf2, "%s", "==> Mario <==");
		lcd_puts(buf2);
	}
	
	int secondary_duration = duration * 85 / total_duration;
	int i = 0;
	for (; i < secondary_duration; i++) 
	{
		SET_BIT(PORTA,0);
		avr_wait(TH);
		CLR_BIT(PORTA,0);
		avr_wait(TL);
	}
}

int get_key() {
	int row, col;
	for (row = 0; row < 4; ++row) {
		for (col = 0; col < 4; ++col) {
			if (is_pressed(row,col)) {
				return (row * 4) + col + 1;
			}
		}
	}
	return 0;
}

int startPlayer(){
	short int choose;
	char messageBuf[17];
	lcd_clr();
	lcd_pos(0,0);
	sprintf(messageBuf, "%s", "Choose a song:");
	lcd_puts(messageBuf);
	avr_wait(30000);
	lcd_clr();
	do {
		lcd_pos(0,0);
		sprintf(messageBuf, "%s", "1 = Twinkle");
		lcd_puts(messageBuf);
		lcd_pos(1,0);
		sprintf(messageBuf, "%s", "2 = Mario");
		lcd_puts(messageBuf);
		avr_wait(100);
		choose = get_key();
	} while (choose!=1 && choose!=2);
	lcd_clr();
	if(choose == 1){
		song_name = 1;
		lcd_pos(0,0);
		sprintf(messageBuf, "%s", "1 = Twinkle");
		lcd_puts(messageBuf);
	}
	else if (choose == 2){
		song_name = 2;
		lcd_pos(0,0);
		sprintf(messageBuf, "%s", "2 = Mario");
		lcd_puts(messageBuf);
	}
	return choose;
}

int is_pressed(int row, int col)
{
	DDRC = 0;
	PORTC = 0;
	SET_BIT(DDRC, row);
	SET_BIT(PORTC, col + 4);
	avr_wait(.1);
	
	if (GET_BIT(PINC,col + 4))
	{
		return 0;
	}
	
	return 1;
}

int main()
{
	avr_init();
	lcd_init();
	DDRA = 0x1F;
	melody = 43;
	char messageBuf[17];
	sprintf(messageBuf, "%s", "Choose a song:");
	lcd_puts(messageBuf);
	avr_wait(30000);
	lcd_clr();
	int chooseSong = startPlayer();
	
	while (1)
	{
		if(chooseSong == 1)
		{
			if (melody < 43)
			{
				struct Note note = song1[melody];
				play_note(FREQUENCY[note.frequency] , DURATION / DURATION_MODIFIER[note.duration]);
				if(note.frequency == 3)
				{
					SET_BIT(PORTA, 1);
					avr_wait(600);
					CLR_BIT(PORTA, 1);
					avr_wait(500);
				}
				else if(note.frequency == 5)
				{
					SET_BIT(PORTA, 2);
					avr_wait(600);
					CLR_BIT(PORTA, 2);
					avr_wait(500);
				}
				else if(note.frequency == 7)
				{
					SET_BIT(PORTA, 3);
					avr_wait(600);
					CLR_BIT(PORTA, 3);
					avr_wait(500);
				}
				else if(note.frequency == 10)
				{
					SET_BIT(PORTA, 4);
					avr_wait(600);
					CLR_BIT(PORTA, 4);
					avr_wait(500);
				}
				melody += 1;
			}
			
			int key = get_key() - 1;
			if (key == -1)
			{
			}
			else if (keypad[key] == '*')
			{
				if (0 < TH_TL_MODIFIER)
				{
					TH_TL_MODIFIER -= .25;
				}
			}
			else if (keypad[key] == '0')
			{
				if (TH_TL_MODIFIER < 2.0)
				{
					TH_TL_MODIFIER += .25;
				}
			}
			else if (keypad[key] == '#')
			{
				if (10 < DURATION)
				{
					DURATION -= 20;
				}
			}
			else if (keypad[key] == 'D')
			{
				if(380 > DURATION)
				{
					DURATION += 20;
				}
				
			}
			else if (keypad[key] == 'A')
			{
				lcd_clr();
				lcd_pos(0,0);
				sprintf(messageBuf, "%s", "Stopped!!!");
				lcd_puts(messageBuf);
				lcd_pos(1,0);
				sprintf(messageBuf, "%s", "Replaying...");
				lcd_puts(messageBuf);
				avr_wait(30000);
				lcd_clr();
				melody = 0;
			}
			else
			melody = 0;
		}
		else 
		{
			if (melody < 13)
			{
				struct Note note = song2[melody];
				play_note(FREQUENCY[note.frequency] , DURATION / DURATION_MODIFIER[note.duration]);
				if(note.frequency == 3)
				{
					SET_BIT(PORTA, 1);
					avr_wait(600);
					CLR_BIT(PORTA, 1);
					avr_wait(500);
				}
				else if(note.frequency == 5)
				{
					SET_BIT(PORTA, 2);
					avr_wait(600);
					CLR_BIT(PORTA, 2);
					avr_wait(500);
				}
				else if(note.frequency == 7)
				{
					SET_BIT(PORTA, 3);
					avr_wait(600);
					CLR_BIT(PORTA, 3);
					avr_wait(500);
				}
				else if(note.frequency == 10)
				{
					SET_BIT(PORTA, 4);
					avr_wait(600);
					CLR_BIT(PORTA, 4);
					avr_wait(500);
				}
				melody += 1;
			}
			
			int key = get_key() - 1;
			if (key == -1)
			{
			}
			else if (keypad[key] == '*')
			{
				if (0 < TH_TL_MODIFIER)
				{
					TH_TL_MODIFIER -= .25;
				}
			}
			else if (keypad[key] == '0')
			{
				if (TH_TL_MODIFIER < 2.0)
				{
					TH_TL_MODIFIER += .25;
				}
			}
			else if (keypad[key] == '#')
			{
				if (10 < DURATION)
				{
					DURATION -= 20;
				}
			}
			else if (keypad[key] == 'D')
			{
				if(380 > DURATION)
				{
					DURATION += 20;
				}
			}
			else if (keypad[key] == 'A')
			{
				lcd_clr();
				lcd_pos(0,0);
				sprintf(messageBuf, "%s", "Stopped!!!");
				lcd_puts(messageBuf);
				lcd_pos(1,0);
				sprintf(messageBuf, "%s", "Replaying...");
				lcd_puts(messageBuf);
				avr_wait(30000);
				lcd_clr();
				melody = 0;
			}
			else
			melody = 0;
		}
	}
		
}
