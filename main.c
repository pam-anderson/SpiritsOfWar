#include "SoW.h"
#include <stdio.h>

int main(void)
{
	music_load("ee2.wav");
	audio_init();
	music_enable_ISR();
	keyboard_init();
	show_menu();
}
