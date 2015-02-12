#include "SoW.h"
#include <stdio.h>

int main(void)
{
	//music_load("ee3.wav");
	//audio_init();
	//music_enable_ISR();
	sdcard_init();
	keyboard_init();
	keyboard_enable_ISR();
	show_menu();
	play_game();
}
