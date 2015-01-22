#include <stdio.h>
#include <stdlib.h>
#include"io.h"
#include "altera_up_avalon_character_lcd.h"
#define buttons (volatile int*) 0x2000

int main()
{
	int i;
	alt_up_character_lcd_dev * char_lcd_dev;
	char_lcd_dev = alt_up_character_lcd_open_dev("/dev/character_lcd_0");
	if(char_lcd_dev == NULL)
		alt_printf ("Error: could not open character LCD device\n");
	else
		alt_printf("Opened character LCD device\n");


	alt_up_character_lcd_init(char_lcd_dev);


	while(1)
	{
		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);

		if(*buttons == 14)
		{
			alt_up_character_lcd_string(char_lcd_dev, "A");
			for(i = 0; i<100000;i++);
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, " ");
		}

		else if(*buttons == 13)
		{
			alt_up_character_lcd_string(char_lcd_dev, "B");
			for(i = 0; i<100000;i++);
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, " ");
		}

		else if(*buttons == 11)
		{
			alt_up_character_lcd_string(char_lcd_dev, "C");
			for(i = 0; i<100000;i++);
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, " ");
		}

		else if(*buttons == 7)
		{
			alt_up_character_lcd_string(char_lcd_dev, "D");
			for(i = 0; i<100000;i++);
			alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);
			alt_up_character_lcd_string(char_lcd_dev, " ");
		}
	}






  return 0;
}
