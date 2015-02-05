#include "SoW.h"
#include "SoW_game_screen.h"

game_tile map[DIMENSION_OF_MAP][DIMENSION_OF_MAP];
player Players[NO_PLAYERS];
int colours[NO_PLAYERS][CHARS_PER_PLAYER] = {{0xF808, 0x7E0, 0x1F,},{ 0xE700, 0xE70, 0xE7 }};
/* Start grids of characters */
int start_pos[NO_PLAYERS][CHARS_PER_PLAYER][2] = {{{0, 6}, {0, 7}, {1, 7}}, {{6, 0}, {7, 0}, {7, 1}}};
/* Coordinates of top left corner of health bar */
int healthbar_pos[NO_PLAYERS][CHARS_PER_PLAYER][2] = {{{29, 20}, {123, 20}, {217, 20}}, {{29, 204}, {123, 204}, {217, 204}}};
class_defaults classes[NUM_OF_CLASSES] = {
		{WARRIOR, WARRIOR_HP, WARRIOR_ATTACK, WARRIOR_DEFENSE, WARRIOR_RANGE},
		{RANGER, RANGER_HP, RANGER_ATTACK, RANGER_DEFENSE, RANGER_RANGE},
		{MAGE, MAGE_HP, MAGE_ATTACK, MAGE_DEFENSE, MAGE_RANGE} };

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

void draw_sprite(int x, int y, sprite type) {
	IOWR_32DIRECT(DRAWER_BASE, 0, x);
	IOWR_32DIRECT(DRAWER_BASE, 4, y);
	IOWR_32DIRECT(DRAWER_BASE, 8, type);
	IOWR_32DIRECT(DRAWER_BASE, 12, 1); //Start
	while(IORD_32DIRECT(DRAWER_BASE, 24) == 0) {}
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

	for(x = 0; x < DIMENSION_OF_MAP; x++, x_coord += SIZE_OF_TILE) {
		y_coord = MAP_CORNER_Y;
		for(y = 0; y < DIMENSION_OF_MAP; y++, y_coord += SIZE_OF_TILE) {
			map[x][y].pos.x = x_coord;
			map[x][y].pos.y = y_coord;
			map[x][y].coords.x = x;
			map[x][y].coords.y = y;
			map[x][y].type = GRASS;
			draw_sprite(x_coord, y_coord, GRASS);
		}
	}
}

void draw_healthbar(int x, int y, int colour) {
	// Draw character
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x + SIZE_OF_TILE/2, y + SIZE_OF_TILE/2, colour, 0);
	// Draw healthbar
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			y + SIZE_OF_TILE/2, 0xF822, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, x + SIZE_OF_TILE/2 + 7, y, x + SIZE_OF_TILE/2 + 9 + HEALTHBAR_LEN,
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
	draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, GRASS);

	if((new_x == -1) || (new_y == -1)) {
		return;
	}
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
		Players[i]->characters_remaining = CHARS_PER_PLAYER;
		for(j = 0; j < CHARS_PER_PLAYER; j++) {
			Players[i]->characters[j].hp = classes[j].hp;
			Players[i]->characters[j].atk = classes[j].atk;
			Players[i]->characters[j].def = classes[j].def;
			Players[i]->characters[j].rng = classes[j].rng;
			Players[i]->characters[j].class = classes[j].class;
			Players[i]->characters[j].colour = colours[i][j];
			Players[i]->characters[j].team = i;
			Players[i]->characters[j].id = j;

			Players[i]->characters[j].pos.x = start_pos[i][j][0];
			Players[i]->characters[j].pos.y = start_pos[i][j][1];
			move_player(i, j, 0, 0, start_pos[i][j][0], start_pos[i][j][1]);

			x = Players[i]->characters[j].pos.x;
			y = Players[i]->characters[j].pos.y;

			alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[x][y].pos.x + 1, map[x][y].pos.y + 1,
					map[x][y].pos.x + SIZE_OF_TILE - 1, map[x][y].pos.y + SIZE_OF_TILE - 1,
					Players[i]->characters[j].colour, 0);
			draw_healthbar(healthbar_pos[i][j][0], healthbar_pos[i][j][1], Players[i]->characters[j].colour);
		}
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

int is_tile_unvisited(int x, int y) {
	if ((0 <= x) && (x < DIMENSION_OF_MAP ) && (0 <= y) && (y < DIMENSION_OF_MAP) &&
			(map[x][y].explored == 0)) {
		return 1;
	} else {
		return 0;
	}
}

void dfs_map(int x, int y, int levels, game_tile** valid_moves) {
	int i;
	int j = 0;
	int k = 1;
	int level;
	int dir;
	game_tile** neighbour;
	game_tile** curr;
	game_tile** tmp;

	neighbour = (game_tile**) calloc(25 , sizeof(game_tile*));
	curr = (game_tile**) calloc(25 , sizeof(game_tile*));
	curr[0] = &map[x][y];
	valid_moves[0] = curr[0];

	// Depth first search on tiles down three levels to get available moves
	for(level = 0; level < levels; level++) {
		// Search each queued neighbour
		j = 0;
		for(i = 0; curr[i] != 0; i++) {
			// Check every neighbour of current tile to see if it would be a valid move
			for(dir = 0; dir < 4; dir++) {
				if(dir == 0) { // LEFT
					if (is_tile_unvisited(curr[i]->coords.x - 1, curr[i]->coords.y) == TRUE) {
						map[curr[i]->coords.x - 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x - 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else if(dir == 1) {// RIGHT
					if (is_tile_unvisited(curr[i]->coords.x + 1, curr[i]->coords.y) == TRUE) {
						map[curr[i]->coords.x + 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x + 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else if(dir == 2) { //UP
					if (is_tile_unvisited(curr[i]->coords.x, curr[i]->coords.y - 1) == TRUE) {
						map[curr[i]->coords.x][curr[i]->coords.y - 1].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x][curr[i]->coords.y - 1];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else { //DOWN
					if (is_tile_unvisited(curr[i]->coords.x, curr[i]->coords.y + 1) == TRUE) {
						map[curr[i]->coords.x][curr[i]->coords.y + 1].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x][curr[i]->coords.y + 1];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
					curr[i] = NULL;
				}
			}
		}
		tmp = curr;
		curr = neighbour;
		neighbour = tmp;
	}
	free(neighbour);
	free(curr);
}

// Depth first search on current node 3 levels to get valid moves
void get_valid_moves(int player_id, int character_id, int x, int y, game_tile** valid_moves) {
	int i;
	int j = 1;
	game_tile **dfs_tree = (game_tile**) calloc(25, sizeof(game_tile*));
	dfs_map(x, y, MAX_SPACES_MOVE, dfs_tree);
	valid_moves[0] = dfs_tree[0];
	dfs_tree[0]->explored = 0;
	for(i = 1; dfs_tree[i] != 0; i++) {
		dfs_tree[i]->explored = 0;
		if (dfs_tree[i]->type == GRASS) {
			valid_moves[j] = dfs_tree[i];
			j++;
		}
	}
	free(dfs_tree);
}

int is_valid_move(int x, int y, game_tile** moves) {
	int i;
	for(i = 0; moves[i] != 0; i++) {
		if (&map[x][y] == moves[i]) {
			return 1;
		}
	}
	return 0;
}

void move_menu(int player_id, int character_id) {
	int i;
	int sel_x = Players[player_id]->characters[character_id].pos.x;
	int sel_y = Players[player_id]->characters[character_id].pos.y;
	game_tile** valid_moves;
	valid_moves = (game_tile**) calloc(25, sizeof(game_tile*));
	get_valid_moves(player_id, character_id, sel_x, sel_y, valid_moves);
	keypress move = get_player_input(SERIAL);

	while(1) {
		if (move == UP) {
			if((sel_y > 0) && is_valid_move(sel_x, sel_y - 1, valid_moves)) {
				sel_y--;
				draw_cursor(sel_x, sel_y + 1, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == DOWN) {
			if((sel_y < DIMENSION_OF_MAP - 1) && is_valid_move(sel_x, sel_y + 1, valid_moves)) {
				sel_y++;
				draw_cursor(sel_x, sel_y - 1, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == LEFT) {
			if((sel_x > 0) && is_valid_move(sel_x - 1, sel_y, valid_moves)) {
				sel_x--;
				draw_cursor(sel_x + 1, sel_y, sel_x, sel_y, Players[player_id]->characters[character_id].colour);
			}
		} else if (move == RIGHT) {
			if((sel_x < DIMENSION_OF_MAP - 1) && is_valid_move(sel_x + 1, sel_y, valid_moves)) {
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
	free(valid_moves);
}

int get_valid_attacks(int player_id, int character_id, game_tile** valid_attacks) {
	int i;
	int j = 1;
	game_tile** dfs_tree = (game_tile**) calloc(25, sizeof(game_tile*));
	dfs_map(Players[player_id]->characters[character_id].pos.x,
			Players[player_id]->characters[character_id].pos.y,
			Players[player_id]->characters[character_id].rng, dfs_tree);
	valid_attacks[0] = dfs_tree[0];
	dfs_tree[0]->explored = 0;
	for(i = 1; dfs_tree[i] != 0; i++) {
		dfs_tree[i]->explored = 0;
		if ((dfs_tree[i]->occupied_by != NULL) && (dfs_tree[i]->occupied_by->team != player_id)) {
			valid_attacks[j] = dfs_tree[i];
			printf("valid atk: %d %d\n", valid_attacks[j]->coords.x, valid_attacks[j]->coords.y);
			j++;
		}
	}
	free(dfs_tree);
	return j;
}

void update_healthbar(int player_id, int character_id) {
	// Black out health lost
	int pixel_per_hp = HEALTHBAR_LEN / classes[Players[player_id]->characters[character_id].class].hp;
	int damage = classes[Players[player_id]->characters[character_id].class].hp -
			Players[player_id]->characters[character_id].hp;
	damage = damage * pixel_per_hp;
	if(damage == 0) {
		return;
	}
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer,
			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN - damage,
			healthbar_pos[player_id][character_id][1] + 1,
			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			healthbar_pos[player_id][character_id][1] + SIZE_OF_TILE/2 - 1, 0x0, 0);
}

void character_is_dead(int player_id, int character_id) {
	// Remove character from map
	move_player(player_id, character_id, Players[player_id]->characters[character_id].pos.x,
			Players[player_id]->characters[character_id].pos.y, -1, -1);
	Players[player_id]->characters_remaining--;
}

void attack_player(int player_id, int character_id, int x, int y) {
	if((x == Players[player_id]->characters[character_id].pos.x) &&
			(y == Players[player_id]->characters[character_id].pos.y)) {
		// If selecting self, it means player chose not to attack
		return;
	}
	map[x][y].occupied_by->hp -= Players[player_id]->characters[character_id].atk;
	if(map[x][y].occupied_by->hp <= 0) {
		// Cannot have negative health
		map[x][y].occupied_by->hp = 0;
		character_is_dead(map[x][y].occupied_by->team, map[x][y].occupied_by->id);
	}
	printf("attacked %d %d ", x, y);
	printf("Player %d character %d attacked player %d and caused %d damage.\n",
			player_id, character_id, map[x][y].occupied_by->team, Players[player_id]->characters[character_id].atk);
}

void attack_menu(int player_id, int character_id) {
	int i = 0;
	int old;
	int atks;
	game_tile** valid_attacks = (game_tile**) calloc(25, sizeof(game_tile*));
	draw_cursor(0, 0, valid_attacks[0]->coords.x, valid_attacks[0]->coords.y,
			Players[player_id]->characters[character_id].colour);
	atks = get_valid_attacks(player_id, character_id, valid_attacks);
	keypress move = get_player_input(SERIAL);

	while(1) {
		if ((move == UP) || (move == RIGHT)) {
			old = i;
			i = i < atks - 1 ? ++i : 0;
			draw_cursor(valid_attacks[old]->coords.x, valid_attacks[old]->coords.y,
					valid_attacks[i]->coords.x, valid_attacks[i]->coords.y,
					Players[player_id]->characters[character_id].colour);

		} else if ((move == DOWN) || (move == LEFT)) {
			old = i;
			i = i - 1 > 0 ? --i : atks - 1;
			draw_cursor(valid_attacks[old]->coords.x, valid_attacks[old]->coords.y,
					valid_attacks[i]->coords.x, valid_attacks[i]->coords.y,
					Players[player_id]->characters[character_id].colour);
		} else if (move == ENTER) {
			//attack_player();
			draw_cursor(valid_attacks[i]->coords.x, valid_attacks[i]->coords.y,
					valid_attacks[i]->coords.x, valid_attacks[i]->coords.y,
					Players[player_id]->characters[character_id].colour);
			attack_player(player_id, character_id, valid_attacks[i]->coords.x, valid_attacks[i]->coords.y);
			update_healthbar(valid_attacks[i]->occupied_by->team, valid_attacks[i]->occupied_by->id);
			break;
		}
		move = get_player_input(SERIAL);
	}
	free(valid_attacks);
}

void play_game() {
	int player_id;
	int character_id;
	int current_move;

	show_game();
	initialize_players();
	position_characters(0);
	position_characters(1);

	 while(1){
		 for(player_id = 0; player_id < NO_PLAYERS; player_id++ ) {
			 for(current_move = MOVE; current_move <= ATTACK; current_move++) {
				 for(character_id = 0; character_id < CHARS_PER_PLAYER; character_id++) {
					 if ((current_move == MOVE) && (Players[player_id]->characters[character_id].hp > 0)) {
						 move_menu(player_id, character_id);
					 } else if ((current_move == ATTACK) && (Players[player_id]->characters[character_id].hp > 0)) {
						 attack_menu(player_id, character_id);
					 } else if (Players[player_id]->characters_remaining == 0) {
						 printf("Game over. Player %d lost!\n", player_id);
						 return;
					 }
				 }
			 }
		 }
	}
	return;
}
