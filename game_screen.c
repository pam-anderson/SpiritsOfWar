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

void draw_healthbar(int x, int y, int colour) {
	// Draw character
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x + SIZE_OF_TILE/2, y + SIZE_OF_TILE/2, colour, 0);
	// Draw healthbar
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			y + SIZE_OF_TILE/2, 0xF822, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, x + SIZE_OF_TILE/2 + 7, y, x + SIZE_OF_TILE/2 + 9 + HEALTHBAR_LEN,
				y + SIZE_OF_TILE/2, 0xFFFF, 0);
}

void draw_characters() {
	int i = 0, k = 0;
	for(i = 0; i < NO_PLAYERS; i++) {
		for(k = 0; k < CHARS_PER_PLAYER; k++) {
			if(Players[i].characters[k].hp > 0) {
				alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[Players[i].characters[k].pos.x][Players[i].characters[k].pos.y].pos.x, map[Players[i].characters[k].pos.x][Players[i].characters[k].pos.y].pos.y,
						map[Players[i].characters[k].pos.x][Players[i].characters[k].pos.y].pos.x + SIZE_OF_TILE - 1, map[Players[i].characters[k].pos.x][Players[i].characters[k].pos.y].pos.y + SIZE_OF_TILE - 1,
							Players[i].characters[k].colour, 0);
			}
		}
	}
}



void draw_sprite(int x, int y, sprite type) {
	IOWR_32DIRECT(DRAWER_BASE, 0, x);
	IOWR_32DIRECT(DRAWER_BASE, 4, y);
	IOWR_32DIRECT(DRAWER_BASE, 8, type);
	IOWR_32DIRECT(DRAWER_BASE, 12, 1); //Start
	//printf("%d, %d\n", IORD_32DIRECT(DRAWER_BASE, 16), IORD_32DIRECT(DRAWER_BASE, 20));
	while(IORD_32DIRECT(DRAWER_BASE, 24) == 0) {}
}

void draw_map() {
	int x = 0, y = 0;
	for(y = 0; y < DIMENSION_OF_MAP; y++) {
		for(x = 0; x < DIMENSION_OF_MAP; x++) {
			draw_sprite(map[x][y].pos.x, map[x][y].pos.y, map[x][y].type);
		}
	}
	draw_characters();
}

void draw_cursor(int old_x, int old_y, int new_x, int new_y, int colour) {
	// Erase old selection
	draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, map[old_x][old_y].type);
	draw_sprite(map[new_x][new_y].pos.x, map[new_x][new_y].pos.y, map[new_x][new_y].type);
	draw_characters();
	//alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[old_x][old_y].pos.x, map[old_x][old_y].pos.y,
	//		map[old_x][old_y].pos.x + SIZE_OF_TILE, map[old_x][old_y].pos.y + SIZE_OF_TILE, 0x3579, 0);
	// Highlight new selection
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[new_x][new_y].pos.x, map[new_x][new_y].pos.y,
			map[new_x][new_y].pos.x + SIZE_OF_TILE - 1, map[new_x][new_y].pos.y + SIZE_OF_TILE - 1, colour, 0);
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
			map[x][y].occupied_by = NULL;
			draw_sprite(x_coord, y_coord, map[x][y].type);
		}
	}
}

void init_timer() {
	int timer_period = 1 * 50000000;
	IOWR_16DIRECT(TIMER_BASE, 8, timer_period & 0xFFFF);
	IOWR_16DIRECT(TIMER_BASE, 12, timer_period >> 16);
	printf(" Stopping Timer\n");
	int status = IORD_16DIRECT(TIMER_BASE, 0);
	if (status & 0x2) {
	IOWR_16DIRECT(TIMER_BASE, 4, 1 << 3);
	}
}

void animate_to_tile(int colour, int dx, int dy, int old_x, int old_y, int new_x, int new_y) {
	int i = 0, j = 0;
	for(i = 0; i < 16; i++) {
		//init_timer();
		//printf(" Starting Timer\n");
		//IOWR_16DIRECT(TIMER_BASE, 4, 1 << 2);
		draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, map[old_x][old_y].type);
		draw_sprite(map[new_x][new_y].pos.x, map[new_x][new_y].pos.y, map[new_x][new_y].type);
		alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[old_x][old_y].pos.x + i * dx, map[old_x][old_y].pos.y + i * dy,
				map[old_x][old_y].pos.x + SIZE_OF_TILE - 1 + i * dx, map[old_x][old_y].pos.y + SIZE_OF_TILE - 1 + i * dy,
				colour, 0);
		j = 0;
		while(j < 10000) {
			j++;
		}
		//printf(" Waiting for timer to expire...\n");
		//int done = 0;
		//while (! done) {
		//	int status = IORD_16DIRECT(TIMER_BASE, 0);
		//	done = status & 0x1;
		//}
	}
}

void animate(int colour, int old_x, int old_y, int new_x, int new_y) {
	while(old_x != new_x || old_y != new_y) {
			if(old_x < new_x && map[old_x + 1][old_y].occupied_by == NULL) {
				animate_to_tile(colour, 1, 0, old_x, old_y, new_x, new_y);
				old_x++;
			}
			else if(old_x > new_x && map[old_x - 1][old_y].occupied_by == NULL) {
				animate_to_tile(colour, -1, 0, old_x, old_y, new_x, new_y);
				old_x--;
			}
			else if(old_y > new_y && map[old_x][old_y - 1].occupied_by == NULL) {
				animate_to_tile(colour, 0, -1, old_x, old_y, new_x, new_y);
				old_y--;
			}
			else if(old_y < new_y && map[old_x][old_y + 1].occupied_by == NULL) {
				animate_to_tile(colour, 0, 1, old_x, old_y, new_x, new_y);
				old_y++;
			}

		}
	//alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[new_x][new_y].pos.x + 1, map[new_x][new_y].pos.y + 1,
	//		map[new_x][new_y].pos.x + SIZE_OF_TILE - 1, map[new_x][new_y].pos.y + SIZE_OF_TILE - 1,
	//		colour, 0);
}

void move_player(int player_id, int char_id, int old_x, int old_y, int new_x, int new_y) {
	// Erase old position
	map[old_x][old_y].occupied_by = NULL;

	if((new_x == -1) || (new_y == -1) || (new_x == DIMENSION_OF_MAP) || (new_y == DIMENSION_OF_MAP)) {
		return;
	}

	animate(Players[player_id].characters[char_id].colour, old_x, old_y, new_x, new_y);

	Players[player_id].characters[char_id].pos.x = new_x;
	Players[player_id].characters[char_id].pos.y = new_y;
	// Place player in new selection
	map[new_x][new_y].occupied_by = &Players[player_id].characters[char_id];
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[new_x][new_y].pos.x + 1, map[new_x][new_y].pos.y + 1,
			map[new_x][new_y].pos.x + SIZE_OF_TILE - 1, map[new_x][new_y].pos.y + SIZE_OF_TILE - 1,
			Players[player_id].characters[char_id].colour, 0);
}

void initialize_players() {
	int i;
	int j;
	int x;
	int y;

	// The boundaries determine the area where players can position their characters
	// when starting the game.
	Players[0].lower_boundary.x = 0;
	Players[0].upper_boundary.x = DIMENSION_OF_MAP/2 - 1;
	Players[0].lower_boundary.y = DIMENSION_OF_MAP/2;
	Players[0].upper_boundary.y = DIMENSION_OF_MAP - 1;

	Players[1].lower_boundary.x = DIMENSION_OF_MAP/2;
	Players[1].upper_boundary.x = DIMENSION_OF_MAP - 1;
	Players[1].lower_boundary.y = 0;
	Players[1].upper_boundary.y = DIMENSION_OF_MAP/2 - 1;

	for(i = 0; i < NO_PLAYERS; i++) {
		Players[i].characters_remaining = CHARS_PER_PLAYER;
		for(j = 0; j < CHARS_PER_PLAYER; j++) {
			Players[i].characters[j].hp = classes[j].hp;
			Players[i].characters[j].atk = classes[j].atk;
			Players[i].characters[j].def = classes[j].def;
			Players[i].characters[j].rng = classes[j].rng;
			Players[i].characters[j].class = classes[j].class;
			Players[i].characters[j].colour = colours[i][j];
			Players[i].characters[j].team = i;
			Players[i].characters[j].id = j;
			Players[i].characters[j].movement = MAX_SPACES_MOVE;

			Players[i].characters[j].pos.x = start_pos[i][j][0];
			Players[i].characters[j].pos.y = start_pos[i][j][1];
			Players[i].characters[j].move = MOVE;

			x = Players[i].characters[j].pos.x;
			y = Players[i].characters[j].pos.y;

			map[x][y].occupied_by = &Players[i].characters[j];
			alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[x][y].pos.x, map[x][y].pos.y,
					map[x][y].pos.x + SIZE_OF_TILE - 1, map[x][y].pos.y + SIZE_OF_TILE - 1,
					Players[i].characters[j].colour, 0);
			draw_healthbar(healthbar_pos[i][j][0], healthbar_pos[i][j][1], Players[i].characters[j].colour);
		}
	}
}

/*
 * @param occupied Set to 1 if an occupied tile is ok, 0 if an occupied tile is not ok
 */
int is_tile_unvisited(int x, int y, int occupied) {
	if ((0 <= x) && (x < DIMENSION_OF_MAP ) && (0 <= y) && (y < DIMENSION_OF_MAP) &&
			!occupied && (map[x][y].occupied_by == NULL) && (map[x][y].explored == 0)) {
		return 1;
	}else if ((0 <= x) && (x < DIMENSION_OF_MAP ) && (0 <= y) && (y < DIMENSION_OF_MAP) &&
			occupied && (map[x][y].explored == 0)) {
		return 1;
	} else {
		return 0;
	}
}

int tile_is_attackable(int player_id, int x, int y) {
	if ((0 <= x) && (x < DIMENSION_OF_MAP) && (0 <= y) && (y < DIMENSION_OF_MAP) &&
			(map[x][y].occupied_by != NULL) && (map[x][y].occupied_by->team != player_id)) {
		return 1;
	} else {
		return 0;
	}
}

int dfs_map(int x, int y, int levels, int occupied, game_tile** valid_moves) {
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
					if (is_tile_unvisited(curr[i]->coords.x - 1, curr[i]->coords.y, occupied) == TRUE) {
						map[curr[i]->coords.x - 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x - 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else if(dir == 1) {// RIGHT
					if (is_tile_unvisited(curr[i]->coords.x + 1, curr[i]->coords.y, occupied) == TRUE) {
						map[curr[i]->coords.x + 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x + 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else if(dir == 2) { //UP
					if (is_tile_unvisited(curr[i]->coords.x, curr[i]->coords.y - 1, occupied) == TRUE) {
						map[curr[i]->coords.x][curr[i]->coords.y - 1].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x][curr[i]->coords.y - 1];
						valid_moves[k] = neighbour[j];
						k++;
						j++;
					}
				} else { //DOWN
					if (is_tile_unvisited(curr[i]->coords.x, curr[i]->coords.y + 1, occupied) == TRUE) {
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
	for(i = 0; i < k; i++) {
		valid_moves[i]->explored = 0;
	}
	free(neighbour);
	free(curr);
	return k;
}

void get_valid_moves(int player_id, int character_id, int x, int y, game_tile** valid_moves) {
	int i;
	int j = 1;
	int len;
	game_tile **dfs_tree = (game_tile**) calloc(25, sizeof(game_tile*));
	len = dfs_map(x, y, MAX_SPACES_MOVE, 0, dfs_tree);
	valid_moves[0] = dfs_tree[0];
	for(i = 1; i < len; i++) {
		if ((dfs_tree[i]->type == GRASS) && dfs_tree[i]->occupied_by == (NULL)) {
			valid_moves[j] = dfs_tree[i];
			j++;
		}
	}
	free(dfs_tree);
}

void get_valid_attack(int player_id, int character_id, int x, int y, game_tile** valid_attacks) {
	int i;
	int j = 1;
	int len;
	game_tile **dfs_tree = (game_tile**) calloc(25, sizeof(game_tile*));
	len = dfs_map(x, y, Players[player_id].characters[character_id].rng, 1, dfs_tree);
	valid_attacks[0] = dfs_tree[0];
	for(i = 1; i < len; i++) {
		if (tile_is_attackable(player_id, dfs_tree[i]->coords.x, dfs_tree[i]->coords.y)) {
			valid_attacks[j] = dfs_tree[i];
			j++;
		}
	}
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

void update_healthbar(int player_id, int character_id) {
	// Black out health lost
	int pixel_per_hp = HEALTHBAR_LEN / classes[Players[player_id].characters[character_id].class].hp;
	int damage = classes[Players[player_id].characters[character_id].class].hp -
			Players[player_id].characters[character_id].hp;
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
	move_player(player_id, character_id, Players[player_id].characters[character_id].pos.x,
			Players[player_id].characters[character_id].pos.y, -1, -1);
	Players[player_id].characters_remaining--;
}

void attack_player(int player_id, int character_id, int x, int y) {
	if((x == Players[player_id].characters[character_id].pos.x) &&
			(y == Players[player_id].characters[character_id].pos.y)) {
		// If selecting self, it means player chose not to attack
		return;
	}
	map[x][y].occupied_by->hp -= Players[player_id].characters[character_id].atk;
	if(map[x][y].occupied_by->hp <= 0) {
		// Cannot have negative health
		map[x][y].occupied_by->hp = 0;
		character_is_dead(map[x][y].occupied_by->team, map[x][y].occupied_by->id);
	}
	printf("Player %d character %d attacked player %d and caused %d damage.\n",
			player_id, character_id, map[x][y].occupied_by->team, Players[player_id].characters[character_id].atk);
	update_healthbar(map[x][y].occupied_by->team, map[x][y].occupied_by->id);
}

void move_cursor(keypress move, int *sel_x, int *sel_y) {
	if (move == UP && *sel_y > 0) {
			*sel_y -= 1;
			draw_cursor(*sel_x, *sel_y + 1, *sel_x, *sel_y, 0xF81F);
	} else if (move == DOWN && *sel_y < DIMENSION_OF_MAP - 1) {
			*sel_y += 1;
			draw_cursor(*sel_x, *sel_y - 1, *sel_x, *sel_y, 0xF81F);
	} else if (move == LEFT && *sel_x > 0) {
			*sel_x -= 1;
			draw_cursor(*sel_x + 1, *sel_y, *sel_x, *sel_y, 0xF81F);
	} else if (move == RIGHT && *sel_x < DIMENSION_OF_MAP - 1) {
			*sel_x += 1;
			draw_cursor(*sel_x - 1, *sel_y, *sel_x, *sel_y, 0xF81F);
	}
}

void select_space(game_tile **valid_moves, int *sel_x, int *sel_y) {
	keypress move = get_player_input(SERIAL);
	draw_cursor(*sel_x, *sel_y, *sel_x, *sel_y, 0xF81F);
	while(1) {
		move_cursor(move, sel_x, sel_y);
		if(move == ENTER && is_valid_move(*sel_x, *sel_y, valid_moves)) {
			return;
		}
		move = get_player_input(SERIAL);
	}
}

void do_movement(int player_id, int character_id) {
	printf("do movement %d %d\n", player_id, character_id);
	int i;
	int old_x = Players[player_id].characters[character_id].pos.x;
	int old_y = Players[player_id].characters[character_id].pos.y;
	int new_x = old_x;
	int new_y = old_y;
	game_tile** valid_moves;
	valid_moves = (game_tile**) calloc(25, sizeof(game_tile*));
	get_valid_moves(player_id, character_id, old_x, old_y, valid_moves);
	for(i = 0; valid_moves[i] != 0; i++) {
		map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type |= 0x100;
	}
	draw_map();
	select_space(valid_moves, &new_x, &new_y);
	move_player(player_id, character_id, old_x, old_y, new_x, new_y);

	for(i = 0; valid_moves[i] != 0; i++) {
		map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type &= 0x0FF;
		//valid_moves[i]->explored = 0;
	}
	draw_map();
	Players[player_id].characters[character_id].move = ATTACK;
	free(valid_moves);
}

void do_attack(int player_id, int character_id) {
	printf("do attack %d %d\n", player_id, character_id);
	int i = 0;
	int old_x = Players[player_id].characters[character_id].pos.x;
	int old_y = Players[player_id].characters[character_id].pos.y;
	int new_x = old_x;
	int new_y = old_y;
	game_tile** valid_attacks;
	valid_attacks = (game_tile**) calloc(25, sizeof(game_tile*));
	get_valid_attack(player_id, character_id, old_x, old_y, valid_attacks);
	for(i = 0; valid_attacks[i] != 0; i++) {
		map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type |= 0x200;
	}
	draw_map();
	select_space(valid_attacks, &new_x, &new_y);
	attack_player(player_id, character_id, new_x, new_y);
	for(i = 0; valid_attacks[i] != 0; i++) {
		map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type &= 0x0FF;
		//valid_attacks[i]->explored = 0;
	}
	draw_map();
	Players[player_id].characters[character_id].move = DONE;
	free(valid_attacks);
}

void select_character(int player_id) {
	printf("select character %d\n", player_id);
	int sel_x = 0, sel_y = 0;
	keypress move = get_player_input(SERIAL);

	while(1) {
		move_cursor(move, &sel_x, &sel_y);
		if(move == ENTER && map[sel_x][sel_y].occupied_by != NULL) {
			if(map[sel_x][sel_y].occupied_by->team == player_id && map[sel_x][sel_y].occupied_by->move == MOVE) {
				do_movement(player_id, map[sel_x][sel_y].occupied_by->id);
				return;
			}
			else if(map[sel_x][sel_y].occupied_by->move == ATTACK) {
				do_attack(player_id, map[sel_x][sel_y].occupied_by->id);
				return;
			}
		}
		move = get_player_input(SERIAL);
	}
}

int is_turn_done(int player_id) {
	int i = 0;

		for(i = 0; i < CHARS_PER_PLAYER; i++) {
			if(Players[player_id].characters[i].move != DONE && Players[player_id].characters[i].hp > 0) {
				return 1;
			}
		}

		return 0;
}

void reset_turn(int player_id) {
	int i = 0;
	for(i = 0; i < CHARS_PER_PLAYER; i++) {
		Players[player_id].characters[i].move = MOVE;
	}
}

void play_game() {
	int player_id = 0;

	show_game();
	initialize_players();
	while(1){
		 while(is_turn_done(player_id)) {
			 if (Players[player_id].characters_remaining == 0) {
				 printf("Game over. Player %d lost!\n", player_id);
				 return;
			 } else {
				 select_character(player_id);
			 }
		 }
		 reset_turn(player_id);
		 player_id = !player_id;

	}
	return;
}
