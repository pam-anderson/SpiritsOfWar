#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

game_tile map[8][8];

/*
 * @brief Draw map to screen.
 */
void show_game(void) {
	int x;
	int y;
	int x_coord = MAP_CORNER_Y;
	int y_coord;

	// Initialize screen. Clear everything.
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, MAP_CORNER_X, MAP_CORNER_Y, MAP_CORNER_X + SIZE_OF_MAP,
			MAP_CORNER_Y + SIZE_OF_MAP, 0x4321, 0);

	// Create tiles
	for(x = 0; x < DIMENSION_OF_MAP; x++, x_coord += SIZE_OF_TILE) {
		y_coord = MAP_CORNER_Y;
		alt_up_pixel_buffer_dma_draw_line(pixel_buffer, x_coord, y_coord, x_coord, y_coord + SIZE_OF_MAP, 0x3579, 0);
		for(y = 0; y < DIMENSION_OF_MAP; y++, y_coord += SIZE_OF_TILE) {
			if(x_coord == MAP_CORNER_X) {
				alt_up_pixel_buffer_dma_draw_line(pixel_buffer, x_coord, y_coord, x_coord + SIZE_OF_MAP, y_coord, 0x3579, 0);
			}
			map[x][y].pos.x = x_coord;
			map[x][y].pos.y = y_coord;
		}
	}
}

void place_players() {

}

void play_game() {
	show_game();
	place_players();
	while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){}
		if(*SERIAL_DATA_LOC == 27) {
			// Exit of game when ESC is pressed
			return;
		}
}

