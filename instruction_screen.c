#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

/*
 * @brief Clear screen and display game play instructions.
 */
void show_instructions() {
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);
	alt_up_char_buffer_string(char_buffer, "Spirits of War", 30, 10);
	alt_up_char_buffer_string(char_buffer, "How to play", 32, 15);
	alt_up_char_buffer_string(char_buffer, "Kill everyone! Text text text text text text text text text text text.", 0, 20);
	alt_up_char_buffer_string(char_buffer, "Kill everyone! Text text text text text text text text text text text.", 0, 22);
	while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){}
		if(*SERIAL_DATA_LOC == ' ') {
            return;
		}
	printf("data: %c\n", *SERIAL_DATA_LOC);
}

