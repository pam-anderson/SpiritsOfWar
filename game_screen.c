#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

game_tile map[8][8];
player Players[2];
int CurrentPlayer = 1;
int CurrentCharacter = 0;
int colours[2][3] = {{0xF808, 0x7E0, 0x1F,},{ 0xE700, 0xE70, 0xE7 }};

#define DEFAULT_HP 		10
#define DEFAULT_ATTACK	3
#define DEFAULT_DEFENSE	5
#define NO_MOVES 2
#define ATK_RNG 1
#define UP	87
#define DOWN 83
#define LEFT 65
#define RIGHT 68
#define HEALTHBAR_LEN 50

typedef enum {
    ATTACK,
    MOVE,
    SKIP
} character_option;


/*
 * @brief Draw map to screen.
 */
void show_game(void) {
	int x;
	int y;
	int x_coord = MAP_CORNER_X;
	int y_coord;

	// Initialize screen. Clear everything.
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, MAP_CORNER_X, MAP_CORNER_Y, MAP_CORNER_X + SIZE_OF_MAP,
			MAP_CORNER_Y + SIZE_OF_MAP, 0x4321, 0);

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

void draw_healthbar(int x, int y, int colour) {
	// Draw character
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x + SIZE_OF_TILE/2, y + SIZE_OF_TILE/2, colour, 0);
	// Draw healthbar
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			y + SIZE_OF_TILE/2, 0xF822, 0);
}

void draw_cursor(int old_x, int old_y, int new_x, int new_y) {
	// Erase old selection
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[old_x][old_y].pos.x, map[old_x][old_y].pos.y,
			map[old_x][old_y].pos.x + SIZE_OF_TILE, map[old_x][old_y].pos.y + SIZE_OF_TILE, 0x3579, 0);
	// Highlight new selection
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[new_x][new_y].pos.x, map[new_x][new_y].pos.y,
			map[new_x][new_y].pos.x + SIZE_OF_TILE, map[new_x][new_y].pos.y + SIZE_OF_TILE, 0x7FF, 0);
}

void move_player(int player_id, int char_id, int old_x, int old_y, int new_x, int new_y) {
	// Erase old selection
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[old_x][old_y].pos.x + 1, map[old_x][old_y].pos.y + 1,
			map[old_x][old_y].pos.x + SIZE_OF_TILE - 1, map[old_x][old_y].pos.y + SIZE_OF_TILE - 1, 0x4321, 0);
	// Highlight new selection
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[new_x][new_y].pos.x + 1, map[new_x][new_y].pos.y + 1,
			map[new_x][new_y].pos.x + SIZE_OF_TILE - 1, map[new_x][new_y].pos.y + SIZE_OF_TILE - 1,
			Players[player_id]->characters[char_id].colour, 0);
}

void initialize_players() {

	int i;
	int j;
	int x;
	int y;
	int healthbar_x;
	int healthbar_y = 20;

	Players[0]->characters[0].pos.x = 0;
	Players[0]->characters[0].pos.y = 7;           //        [ ][ ] [ ][ ] [ ][ ] [0][1]
	                                               	   	   	     //        [ ][ ] [ ][ ] [ ][ ] [ ][2]
	                                               	   	   	     //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[1].pos.x = 0;	         //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[1].pos.y = 6;	         //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	                                    	       	   	   	     //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[2].pos.x = 1;           //        [0][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[2].pos.y = 7;		     //        [1][2] [ ][ ] [ ][ ] [ ][ ]

	Players[1]->characters[0].pos.x = 7;
	Players[1]->characters[0].pos.y = 0;

	Players[1]->characters[1].pos.x = 6;
	Players[1]->characters[1].pos.y = 0;

	Players[1]->characters[2].pos.x = 7;
	Players[1]->characters[2].pos.y = 1;

	for(i = 0; i < NO_PLAYERS; i++) {
		healthbar_x = 29;

		for(j = 0; j < CHARS_PER_PLAYER; j++, healthbar_x += 94) {
			Players[i]->characters[j].hp = DEFAULT_HP;
			Players[i]->characters[j].atk = DEFAULT_ATTACK;
			Players[i]->characters[j].def = DEFAULT_DEFENSE;
			Players[i]->characters[j].colour = colours[i][j];
			x = Players[i]->characters[j].pos.x;
			y = Players[i]->characters[j].pos.y;

			alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[x][y].pos.x + 1, map[x][y].pos.y + 1,
					map[x][y].pos.x + SIZE_OF_TILE - 1, map[x][y].pos.y + SIZE_OF_TILE - 1,
					Players[i]->characters[j].colour, 0);
			draw_healthbar(healthbar_x, healthbar_y, Players[i]->characters[j].colour);
		}
		healthbar_y = 204;
	}
}

void player_attack(int player_id)
{
		int	sel_x;
		int sel_y;
		int i;
		int enemy = CurrentPlayer ^ 1;

		for(i = 0; i < CHARS_PER_PLAYER; i++) {
			if(Players[player_id]->characters[i].hp > 0) {
				while(1) {
				sel_x = Players[player_id]->characters[i].pos.x;
				sel_y = Players[player_id]->characters[i].pos.y;
				while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){
										  // Wait for character to come in
				}
							if(*SERIAL_DATA_LOC == 'w') { // MOVE UP
								if(sel_y > 0) {
									sel_y--;
									Players[player_id]->characters[i].pos.y--;
									move_player(player_id, i, sel_x, sel_y + 1, sel_x, sel_y);
								}
							} else if (*SERIAL_DATA_LOC == 's') { // MOVE DOWN
								if(sel_y < DIMENSION_OF_MAP) {
									sel_y++;
									Players[player_id]->characters[i].pos.y++;
									move_player(player_id, i, sel_x, sel_y - 1, sel_x, sel_y);
								}
							} else if (*SERIAL_DATA_LOC == 'a') { // MOVE LEFT
								if(sel_x > 0) {
									sel_x--;
									Players[player_id]->characters[i].pos.x--;
									move_player(player_id, i, sel_x + 1, sel_y, sel_x, sel_y);
								}
							} else if (*SERIAL_DATA_LOC == 'd') { // MOVE RIGHT
								if(sel_x < DIMENSION_OF_MAP) {
									sel_x++;
									Players[player_id]->characters[i].pos.x++;
									move_player(player_id, i, sel_x - 1, sel_y, sel_x, sel_y);
								}
							} else if (*SERIAL_DATA_LOC == ' ') { // PLACE PLAYER
								break;
							}
							printf("data: %c\n", *SERIAL_DATA_LOC);
			}}
		}
}

void move_menu() {
	int sel_x = 0;
	int sel_y = 0;

	while(1) {
		while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){
						  // Wait for character to come in
		}
			if(*SERIAL_DATA_LOC == 'w') { // MOVE UP
				if(sel_y > 0) {
					sel_y--;
					draw_cursor(sel_x, sel_y + 1, sel_x, sel_y);
				}
			} else if (*SERIAL_DATA_LOC == 's') { // MOVE DOWN
				if(sel_y < DIMENSION_OF_MAP) {
					sel_y++;
					draw_cursor(sel_x, sel_y - 1, sel_x, sel_y);
				}
			} else if (*SERIAL_DATA_LOC == 'a') { // MOVE LEFT
				if(sel_x > 0) {
					sel_x--;
					draw_cursor(sel_x + 1, sel_y, sel_x, sel_y);
				}
			} else if (*SERIAL_DATA_LOC == 'd') { // MOVE RIGHT
				if(sel_x < DIMENSION_OF_MAP) {
					sel_x++;
					draw_cursor(sel_x - 1, sel_y, sel_x, sel_y);
				}
			}
			printf("data: %c\n", *SERIAL_DATA_LOC);
	}
}

void play_game() {
	show_game();
	initialize_players();
	draw_cursor(0, 0, 0, 0);

	 while(1){
	     player_attack(0);
	     player_attack(1);
		 move_menu();

	/*	CurrentPlayer = CurrentPlayer ^ 1;
		TurnDone = 0;

		for(CurrentCharacter = 0; CurrentCharacter<CHARS_PER_PLAYER; CurrentCharacter++) {
			if(Players[CurrentPlayer]->characters[i].hp <= 0){}
			else {
				// Display Menu Here for Move, Attack, Skip...
				cursor = ATTACK;
				if(CurrentPlayer == 0)
					keyboard_read();
				else if (CurrentPlayer == 1) {
					// Read from Serial Instead...
					// Maybe *KeyInput = *SERIAL_DATA_LOC
				}

				while(1) {
					if(*KeyInput == UP) {
						if(cursor == ATTACK){}
						else
							cursor++;
					} else if (*KeyInput == DOWN) {
						if(cursor == SKIP){}
						else
							cursor--;
					} else if (*KeyInput == ' ') {
						break;
					}
				}

				if( cursor == ATTACK)
					attack_menu();
				else if(cursor == MOVE)
					move_menu();
				// Do nothing if Skip
			}
		}*/
	}
	return;
}
