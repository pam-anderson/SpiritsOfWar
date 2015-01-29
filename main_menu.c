#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

typedef enum {
    START,
    INSTR
} menu_option;

int menu_pos[2] = {120, 140};

/*
 * @brief Draws the start menu to the screen.
 */
void draw_menu() {
	// Initialize
	  char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");
	  alt_up_char_buffer_init(char_buffer);
	  alt_up_char_buffer_clear(char_buffer);
	  pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/pixel_buffer_dma");

	  alt_up_char_buffer_string(char_buffer, "Spirits of War", 30, 10);
	  alt_up_char_buffer_string(char_buffer, "Start game", 30, 30);
	  alt_up_char_buffer_string(char_buffer, "Instructions", 29, 35);
	  alt_up_char_buffer_string(char_buffer, "W - Up    A - Left", 0, 55);
	  alt_up_char_buffer_string(char_buffer, "S - Down  D - Right", 0, 56);

	  alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer, PIXEL_BUFFER_BASE);
	  alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);

	  while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	  alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
}

/*
 * @brief Move selection arrow between menu options. Uses positions based off menu_pos array.
 *
 * @param curr_position -- Old position of selection arrow - index of menu_pos array
 * @param new_position -- New position of selection arrow - index of menu_pos array
 */
void move_arrow(int curr_position, int new_position) {
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 110, menu_pos[curr_position], 114, menu_pos[curr_position] + 4, 0x0000, 0);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, 110, menu_pos[new_position], 114, menu_pos[new_position] + 4, 0x3456, 0);
}

/*
 * @brief Draw the main menu, then wait on key presses to move selection arrow and change screens.
 */
void show_menu() {
	  menu_option menu = START;

	  draw_menu();
	  move_arrow(menu, menu);
	  serial_port = alt_up_rs232_open_dev(RS232_NAME);
	  // Clear FIFO
	  alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC);

	  while(1){
		  while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){
			  // Wait for character to come in
		  }

		  if((*SERIAL_DATA_LOC == 'w') || (*SERIAL_DATA_LOC == 's')) {
			  // Navigating up/down the menu
			  if(menu == START) {
				  menu = INSTR;
				  move_arrow(START, INSTR);
			  } else {
				  menu = START;
				  move_arrow(INSTR, START);
			  }
		  } else if (*SERIAL_DATA_LOC == ' ') {
			  // Menu option selected
			  if(menu == INSTR) {
				  show_instructions();
			  } else {
				  play_game();
			  }
			  draw_menu();
			  move_arrow(menu, menu);
		  }
		  printf("data: %c\n", *SERIAL_DATA_LOC);
	  }
}


