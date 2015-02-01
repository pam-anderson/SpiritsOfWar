#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

game_tile map[DIMENSION_OF_MAP][DIMENSION_OF_MAP];
player Players[NO_PLAYERS];
int CurrentPlayer = 1;
int CurrentCharacter = 0;
int colours[NO_PLAYERS][CHARS_PER_PLAYER] = {{0xF808, 0x7E0, 0x1F,},{ 0xE700, 0xE70, 0xE7 }};
int start_pos[NO_PLAYERS][CHARS_PER_PLAYER][2] = {{{0, 6}, {0, 7}, {1, 7}}, {{6, 0}, {7, 0}, {7, 1}}};

#define MAX_SPACES_MOVE 3
#define DEFAULT_HP 		10
#define DEFAULT_ATTACK	3
#define DEFAULT_DEFENSE	5
#define NO_MOVES        2
#define ATK_RNG         1
#define HEALTHBAR_LEN   50
#define SERIAL          0
#define KEYBOARD        1

typedef enum {
    MOVE,
    ATTACK
} character_option;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ENTER,
	ESC
} keypress;


keypress get_player_input(int type) {
	if(type == SERIAL) {
		while(1) {
			while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){
				// Wait for character to come in
			}
			if (*SERIAL_DATA_LOC == 'w') {
				return UP;
			} else if (*SERIAL_DATA_LOC == 's') {
				return DOWN;
			} else if (*SERIAL_DATA_LOC == 'a') {
				return LEFT;
			} else if (*SERIAL_DATA_LOC == 'd') {
				return RIGHT;
			} else if (*SERIAL_DATA_LOC == ' ') {
				return ENTER;
			} else if (*SERIAL_DATA_LOC == 27) {
				return ESC;
			}
		}
	} else {
		// get keyboard input
	}
}

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
			map[x][y].type = GRASS;
		}
	}
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, x_coord, y_coord, x_coord, y_coord - SIZE_OF_MAP, 0x3579, 0);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, x_coord, y_coord, x_coord - SIZE_OF_MAP, y_coord, 0x3579, 0);
}

void draw_healthbar(int x, int y, int colour) {
	// Draw character
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x + SIZE_OF_TILE/2, y + SIZE_OF_TILE/2, colour, 0);
	// Draw healthbar
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			y + SIZE_OF_TILE/2, 0xF822, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
				y + SIZE_OF_TILE/2, 0xFFFF, 0);
}

void draw_cursor(int old_x, int old_y, int new_x, int new_y, int colour) {
	// Erase old selection
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[old_x][old_y].pos.x, map[old_x][old_y].pos.y,
			map[old_x][old_y].pos.x + SIZE_OF_TILE, map[old_x][old_y].pos.y + SIZE_OF_TILE, 0x3579, 0);
	// Highlight new selection
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[new_x][new_y].pos.x, map[new_x][new_y].pos.y,
			map[new_x][new_y].pos.x + SIZE_OF_TILE, map[new_x][new_y].pos.y + SIZE_OF_TILE, colour, 0);
}

void move_player(int player_id, int char_id, int old_x, int old_y, int new_x, int new_y) {
	// Erase old position
	map[old_x][old_y].type = GRASS;
	map[old_x][old_y].occupied_by = NULL;
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[old_x][old_y].pos.x + 1, map[old_x][old_y].pos.y + 1,
			map[old_x][old_y].pos.x + SIZE_OF_TILE - 1, map[old_x][old_y].pos.y + SIZE_OF_TILE - 1, 0x4321, 0);
	// Place player in new selection
	map[new_x][new_y].type = CHARACTER;
	map[new_x][new_y].occupied_by = &Players[player_id]->characters[char_id];
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

	// The boundaries determine the area where players can position their characters
	// when starting the game.
	Players[0]->lower_boundary.x = 0;
	Players[0]->upper_boundary.x = DIMENSION_OF_MAP/2 - 1;
	Players[0]->lower_boundary.y = DIMENSION_OF_MAP/2;
	Players[0]->upper_boundary.y = DIMENSION_OF_MAP - 1;

	Players[1]->lower_boundary.x = DIMENSION_OF_MAP/2;
	Players[1]->upper_boundary.x = DIMENSION_OF_MAP - 1;
	Players[1]->lower_boundary.y = 0;
	Players[1]->upper_boundary.y = DIMENSION_OF_MAP/2 - 1;

	for(i = 0; i < NO_PLAYERS; i++) {
		healthbar_x = 29;

		for(j = 0; j < CHARS_PER_PLAYER; j++, healthbar_x += 94) {
			Players[i]->characters[j].hp = DEFAULT_HP;
			Players[i]->characters[j].atk = DEFAULT_ATTACK;
			Players[i]->characters[j].def = DEFAULT_DEFENSE;
			Players[i]->characters[j].colour = colours[i][j];

			Players[i]->characters[j].pos.x = start_pos[i][j][0];
			Players[i]->characters[j].pos.y = start_pos[i][j][1];
			move_player(i, j, 0, 0, start_pos[i][j][0], start_pos[i][j][1]);

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

void position_characters(int player_id)
{
	int	sel_x;
	int sel_y;
	int i;
	keypress move;

	for(i = 0; i < CHARS_PER_PLAYER; i++) {
		if(Players[player_id]->characters[i].hp > 0) {
			sel_x = Players[player_id]->characters[i].pos.x;
			sel_y = Players[player_id]->characters[i].pos.y;
			move = get_player_input(SERIAL);
			while(move != ENTER) {
				if (move == UP) {
					if((sel_y > Players[player_id]->lower_boundary.y) && (map[sel_x][sel_y - 1].type == GRASS)){
						sel_y--;
						Players[player_id]->characters[i].pos.y--;
						move_player(player_id, i, sel_x, sel_y + 1, sel_x, sel_y);
					}
				} else if (move == DOWN) {
					if((sel_y < Players[player_id]->upper_boundary.y) && (map[sel_x][sel_y + 1].type == GRASS)) {
						sel_y++;
						Players[player_id]->characters[i].pos.y++;
						move_player(player_id, i, sel_x, sel_y - 1, sel_x, sel_y);
					}
				} else if (move == LEFT) {
					if((sel_x > Players[player_id]->lower_boundary.x) && (map[sel_x - 1][sel_y].type == GRASS)) {
						sel_x--;
						Players[player_id]->characters[i].pos.x--;
						move_player(player_id, i, sel_x + 1, sel_y, sel_x, sel_y);
						}
				} else if (move == RIGHT) {
					if((sel_x < Players[player_id]->upper_boundary.x) && (map[sel_x + 1][sel_y].type == GRASS)) {
						sel_x++;
						Players[player_id]->characters[i].pos.x++;
						move_player(player_id, i, sel_x - 1, sel_y, sel_x, sel_y);
					}
				}
				move = get_player_input(SERIAL);
			}
		}
	}
}

int is_valid_move(int player_id, int character_id, int x, int y) {
	int total = 0;

	// Is the map occupied by another player or is it a grass tile?
	if((map[x][y].type != GRASS) && (map[x][y].occupied_by != &Players[player_id]->characters[character_id])) {
		return 0;
	}

	// Is the total spaces to move no more than the maximum allowed spaces to move?
	total = abs(Players[player_id]->characters[character_id].pos.x - x);
	total += abs(Players[player_id]->characters[character_id].pos.y - y);
	if(total <= MAX_SPACES_MOVE) {
		return 1;
	} else {
		return 0;
	}
}

void move_menu(int player_id, int character_id) {
	int sel_x = Players[player_id]->characters[character_id].pos.x;
	int sel_y = Players[player_id]->characters[character_id].pos.y;
	keypress move = get_player_input(SERIAL);

	while(1) {
		if (move == UP) {
			if((sel_y > 0) && is_valid_move(player_id, character_id, sel_x, sel_y - 1)) {
				sel_y--;
				draw_cursor(sel_x, sel_y + 1, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == DOWN) {
			if((sel_y < DIMENSION_OF_MAP - 1) && is_valid_move(player_id, character_id, sel_x, sel_y + 1)) {
				sel_y++;
				draw_cursor(sel_x, sel_y - 1, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == LEFT) {
			if((sel_x > 0) && is_valid_move(player_id, character_id, sel_x - 1, sel_y)) {
				sel_x--;
				draw_cursor(sel_x + 1, sel_y, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == RIGHT) {
			if((sel_x < DIMENSION_OF_MAP - 1) && is_valid_move(player_id, character_id, sel_x + 1, sel_y)) {
				sel_x++;
				draw_cursor(sel_x - 1, sel_y, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == ENTER) {
			move_player(player_id, character_id, Players[player_id]->characters[character_id].pos.x,
					Players[player_id]->characters[character_id].pos.y, sel_x, sel_y);
			draw_cursor(sel_x, sel_y, sel_x, sel_y, 0x3579);
			Players[player_id]->characters[character_id].pos.x = sel_x;
			Players[player_id]->characters[character_id].pos.y = sel_y;
			break;
		}
		move = get_player_input(SERIAL);
	}
}

void play_game() {
	int player_id;
	int character_id;
	int current_move;

	show_game();
	initialize_players();
	//draw_cursor(0, 0, 0, 0);
	position_characters(0);
	position_characters(1);

	 while(1){
		 for(player_id = 0; player_id < NO_PLAYERS; player_id++ ) {
			 for(current_move = MOVE; current_move <= ATTACK; current_move++) {
				 for(character_id = 0; character_id < CHARS_PER_PLAYER; character_id++) {
					 if (current_move == MOVE) {
						 move_menu(player_id, character_id);
					 } else {
					//	 attack_menu(player_id, character_id);
					 }
				 }
			 }
		 }
	}
	return;
}
