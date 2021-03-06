#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

/*
 * @brief Clear screen and display game play instructions.
 */
void show_instructions() {
	keypress key;
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);
	alt_up_char_buffer_string(char_buffer, "Spirits of War", 30, 10);
	alt_up_char_buffer_string(char_buffer, "How to play", 32, 15);
	alt_up_char_buffer_string(char_buffer, "Your goal is to eliminate the enemy team!", 0, 20);
	alt_up_char_buffer_string(char_buffer, "Navigate the on-screen prompts with 'W' for Up, 'S' for Down, 'A' For Left, 'D' for Right and '/' to confirm!", 0, 22);
	alt_up_char_buffer_string(char_buffer, "To exit at any time press ']'", 0, 24);
	alt_up_char_buffer_string(char_buffer, "Good Luck!", 0, 28);
	while(1) {
		key = get_player_input(0);
		if((key == ENTER) || (key == ESC)) {
			return;
		}
	}
}

