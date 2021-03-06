#include "SoW.h"
#include "Sow_game_screen.h"

 /*
  *@brief Initialize the hardware to hold textures (grass, water, rock, etc.)
  */
 void hardware_init() {
	int i = 0, j = 0;
	for(i = GRASS; i <= 2; i++) {
		IOWR_32DIRECT(DRAWER_BASE, 8, i);
		for(j = 0; j < 16 * 16; j++) {
			switch(i) {
				case GRASS: IOWR_32DIRECT(DRAWER_BASE, 16, grass[j]);
						break;
				case WATER: IOWR_32DIRECT(DRAWER_BASE, 16, water[j]);
						break;
				case ROCK: IOWR_32DIRECT(DRAWER_BASE, 16, rock[j]);
						break;
			}
			while(IORD_32DIRECT(DRAWER_BASE, 24) == 0) {};
		}
	}
}

/*
 *@brief Loads a particular sprite into the ram location
 *@param player_id The players sprite that needs to be loaded
 *@param character_id The character that holds the sprite
 *@param ram_location The ram address to write the sprite to
 *@param type The sprite type in the character struct
 */
void load_sprite_hardware(int player_id, int character_id, int ram_location, int type) {
	int j = 0;
	IOWR_32DIRECT(DRAWER_BASE, 8, ram_location);
	for(j = 0; j < 16 * 16; j++) {
		IOWR_32DIRECT(DRAWER_BASE, 16, Players[player_id]->characters[character_id].sprites[type][j]);
		while(IORD_32DIRECT(DRAWER_BASE, 24) == 0) {};
	}
}

/*
 *@brief Loads the sprites into the character struct calling load sprite which reads the sdcard
 *@param player_id The player id
 *@param character_id The character sprites we are reading
 */
void sprite_init(int player_id, int character_id) {
	int i = 0;
	Players[player_id]->characters[character_id].sprites = (int **) calloc(9, sizeof(int *));
	for(i = 0; i < SPRITES_PER_CHAR; i++) {
		Players[player_id]->characters[character_id].sprites[i] = (int *) calloc(256, sizeof(int));
		load_sprite(filenames[player_id * CHARS_PER_PLAYER + character_id][i], Players[player_id]->characters[character_id].sprites[i]);
	}
}

/*
 *@brief Loads the animation sprites needed for the players turn into ram
 *@param player_id The current players turn
 */
void load_turn(int player_id) {
	int i = 0, j = 0;
	for(i = 0; i < CHARS_PER_PLAYER; i++) {
		for(j = 1; j < SPRITES_PER_CHAR; j++) {
			load_sprite_hardware(player_id, i, i * (SPRITES_PER_CHAR - 1) + ANIMATION_HARDWARE + (j - 1), j);
		}
	}
}

/*
 * @brief Perform a depth first search to a defined amount of levels on the map, starting at
 *        a specific tile. Return a list of all valid, explored tiles.
 * @param player_id Player id
 * @param character_id Character id
 * @param x Map coordinate in the x axis
 * @param y Map coordinate in the y axis
 * @param levels The number of levels to explore in the search
 * @param valid_moves An empty list where the explored tiles will be store
 * @param is_valid A function which takes the current tile and determines if it is valid
 */
 
void dfs_map(int player_id, int character_id, int x, int y, int levels,game_tile** valid_moves,
		int (*is_valid)(int, int, int)) {
	int level;
	int i;
	int j = 0;
	int k = 1;
	int dir;
	game_tile** neighbour;
	game_tile** curr;
	game_tile** tmp;

	neighbour = (game_tile**) calloc(75 , sizeof(game_tile*));
	curr = (game_tile**) calloc(75 , sizeof(game_tile*));
	curr[0] = &map[x][y];
	valid_moves[0] = curr[0];
	map[x][y].distance = 0;

	// Depth first search on tiles down three levels to get available moves
	for(level = 0; level < levels; level++) {
		// Search each queued neighbour
		j = 0;
		for(i = 0; curr[i] != 0; i++) {
			// Check every neighbour of current tile to see if it would be a valid move
			for(dir = 0; dir < 4; dir++) {
				if(dir == 0) { // LEFT
					if ((*is_valid)(player_id, curr[i]->coords.x - 1, curr[i]->coords.y) == TRUE) {
						map[curr[i]->coords.x - 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x - 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						valid_moves[k]->distance = level + 1;
						k++;
						j++;
					}
				} else if(dir == 1) {// RIGHTS
					if ((*is_valid)(player_id, curr[i]->coords.x + 1, curr[i]->coords.y) == TRUE) {
						map[curr[i]->coords.x + 1][curr[i]->coords.y].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x + 1][curr[i]->coords.y];
						valid_moves[k] = neighbour[j];
						valid_moves[k]->distance = level + 1;
						k++;
						j++;
					}
				} else if(dir == 2) { //UP
					if ((*is_valid)(player_id, curr[i]->coords.x, curr[i]->coords.y - 1) == TRUE) {
						map[curr[i]->coords.x][curr[i]->coords.y - 1].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x][curr[i]->coords.y - 1];
						valid_moves[k] = neighbour[j];
						valid_moves[k]->distance = level + 1;
						k++;
						j++;
					}
				} else { //DOWN
					if ((*is_valid)(player_id, curr[i]->coords.x, curr[i]->coords.y + 1) == TRUE) {
						map[curr[i]->coords.x][curr[i]->coords.y + 1].explored = 1;
						neighbour[j] = &map[curr[i]->coords.x][curr[i]->coords.y + 1];
						valid_moves[k] = neighbour[j];
						valid_moves[k]->distance = level + 1;
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

/*
 * @brief Draw a specific sprite to the screen using the pixel_drawer accelerator.
 * @param x The absolute position of the pixel in the x axis of the top left corner of the sprite
 * @param y The absolute position of the pixel in the y axis of the top left corner of the sprite
 * @param type The sprite to draw
 */
void draw_sprite(int x, int y, sprite type) {
	IOWR_32DIRECT(DRAWER_BASE, 0, x);
	IOWR_32DIRECT(DRAWER_BASE, 4, y);
	IOWR_32DIRECT(DRAWER_BASE, 8, type);
	IOWR_32DIRECT(DRAWER_BASE, 12, 1); //Start
	while(IORD_32DIRECT(DRAWER_BASE, 24) == 0) {}
	if(type == GRASS) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x, y, grass[0], 0);
	else if(type == WATER) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x, y, water[0], 0);
	else if(type == ROCK) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x, y, rock[0], 0);
	else if(type == (GRASS|0x200)) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x, y, grass[0]|0xF800, 0);
	else if(type == (GRASS|0x100) || (type & 0xFF) >= ANIMATION_HARDWARE) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x, y, grass[0]|0x001F, 0);
}
/*
 * @brief Draw a tile corner pixel
 * @param x The map x tile that needs drawing
 * @param y The map y tile that needs drawing
 * @param type The type of tile
 */
void draw_corner(int px, int py, sprite type) {
	if(map[px][py].type == GRASS) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[px][py].pos.x, map[px][py].pos.y, map[px][py].pos.x, map[px][py].pos.y, grass[0], 0);
	else if(map[px][py].type == WATER) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[px][py].pos.x, map[px][py].pos.y, map[px][py].pos.x, map[px][py].pos.y, water[0], 0);
	else if(map[px][py].type == ROCK) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[px][py].pos.x, map[px][py].pos.y, map[px][py].pos.x, map[px][py].pos.y, rock[0], 0);
	else if(map[px][py].type == (GRASS|0x100)) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[px][py].pos.x, map[px][py].pos.y, map[px][py].pos.x, map[px][py].pos.y, grass[0]|0x1F, 0);
	else if(map[px][py].type == (GRASS|0x200)) alt_up_pixel_buffer_dma_draw_box(pixel_buffer, map[px][py].pos.x, map[px][py].pos.y, map[px][py].pos.x, map[px][py].pos.y, grass[0]|0xF800, 0);
}

/*
 * @brief Draw the initial health bar at a given position, as well as the character associated with
 *        that health bar
 * @param x The absolute position of the pixel in the x axis of the top left corner of the character
 * 		  to be drawn beside the health bar.
 * @param y The absolute position of the pixel in the y axis of the top left corner of the character
 * 		  to be drawn beside the health bar.
 */
void draw_healthbar(int player_id, int character_id, int x, int y) {
	// Draw character
	draw_sprite(x - 8, y - 4, Players[player_id]->characters[character_id].standing);
	//alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x, y, x + SIZE_OF_TILE/2, y + SIZE_OF_TILE/2, colour, 0);
	// Draw healthbar
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer, x + SIZE_OF_TILE/2 + 8, y, x + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			y + SIZE_OF_TILE/2, 0xF822, 0);
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, x + SIZE_OF_TILE/2 + 7, y, x + SIZE_OF_TILE/2 + 9 + HEALTHBAR_LEN,
				y + SIZE_OF_TILE/2, 0xFFFF, 0);
}

/*
 * @brief Draw every alive character to the map
 */
void draw_characters() {
	int i = 0, k = 0;
	for(i = 0; i < NO_PLAYERS; i++) {
		for(k = 0; k < CHARS_PER_PLAYER; k++) {
			if(Players[i]->characters[k].hp > 0) {
				int px = Players[i]->characters[k].pos.x;
				int py = Players[i]->characters[k].pos.y;
				draw_sprite(map[px][py].pos.x, map[px][py].pos.y, Players[i]->characters[k].standing);
				draw_corner(px, py, map[px][py].type);
			}
		}
	}
}

/*
 * @brief Draw the entire map and the alive characters.
 */
void draw_map() {
	int x = 0, y = 0;
	for(y = 0; y < DIMENSION_OF_MAP_Y; y++) {
		for(x = 0; x < DIMENSION_OF_MAP_X; x++) {
			draw_sprite(map[x][y].pos.x, map[x][y].pos.y, map[x][y].type);
		}
	}
	draw_characters();
}

/*
 * @brief Move the selection cursor to a new position
 * @param old_x The current x coordinate of the cursor
 * @param old_y The current y coordinate of the cursor
 * @param new_x The new x coordinate of the cursor
 * @param new_y The new y coordinate of the cursor
 */
void draw_cursor(int old_x, int old_y, int new_x, int new_y, int colour) {
	draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, map[old_x][old_y].type);
	draw_sprite(map[new_x][new_y].pos.x, map[new_x][new_y].pos.y, map[new_x][new_y].type);
	draw_characters();
	alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer, map[new_x][new_y].pos.x, map[new_x][new_y].pos.y,
			map[new_x][new_y].pos.x + SIZE_OF_TILE - 1, map[new_x][new_y].pos.y + SIZE_OF_TILE - 1, colour, 0);
}

/*
 * @brief Draws exit prompt to screen
 * @param player_id Player id
 * @return 0 if player chose to exit, 1 if player did not choose to exit
 */
int draw_exit_screen(int player_id) {
	// Use player id for input
	keypress key;
	alt_up_char_buffer_clear(char_buffer);
	alt_up_char_buffer_string(char_buffer, "Are you sure you want to quit?", 25, 23);
	alt_up_char_buffer_string(char_buffer, "[A] - Yes     [D] - No", 30, 25);
	while(1) {
		key = get_player_input(player_id);
		if (key == LEFT) {
			alt_up_char_buffer_clear(char_buffer);
			return 0;
		} else if (key == RIGHT) {
			alt_up_char_buffer_clear(char_buffer);
			return 1;
		}
	}
}

/*
 * @brief Find out what the current move is for a player
 * @param player_id Player id
 * @return The player's current move of type character_option
 */
character_option find_player_move(int player_id) {
	int character_id;
	character_option curr_move = DONE;
	for(character_id = 0; character_id < CHARS_PER_PLAYER; character_id++) {
		if(Players[player_id]->characters[character_id].move < curr_move) {
			curr_move = Players[player_id]->characters[character_id].move;
		}
	}
	return curr_move;
}

/*
 * @brief ISR for the alarm. Executes every 0.25 seconds. Draws coloured box around
 *        icons of characters that are able to take a turn.
 */
alt_u32 alarm_blink_isr(void* context) {
	int character_id;
	int player_id;
	char colour;
	character_option curr_move = find_player_move(main_player_id);
	blinker = ~blinker;
	for(player_id = 0; player_id < NO_PLAYERS; player_id++) {
		if(player_id != main_player_id) {
			colour = 0;
		} else {
			colour = 0xFFFF;
		}
		for(character_id = 0; character_id < CHARS_PER_PLAYER; character_id++) {
			if(Players[player_id]->characters[character_id].move == curr_move) {
				alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer,
					healthbar_pos[player_id][character_id][0] - SIZE_OF_TILE/2 - 1,
					healthbar_pos[player_id][character_id][1] - SIZE_OF_TILE/2 + 1,
					healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2,
					healthbar_pos[player_id][character_id][1] + SIZE_OF_TILE/2 + 3,
					blinker & colour, 0);
			} else {
				alt_up_pixel_buffer_dma_draw_rectangle(pixel_buffer,
					healthbar_pos[player_id][character_id][0] - SIZE_OF_TILE/2 - 1,
					healthbar_pos[player_id][character_id][1] - SIZE_OF_TILE/2 + 1,
					healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2,
					healthbar_pos[player_id][character_id][1] + SIZE_OF_TILE/2 + 3,
					0, 0);
			}
		}
	}
	return alt_ticks_per_second() / 4;
}

/*
 * @brief Initialize sysclk to interrupt every .25 seconds and set up ISR.
 */
void initialize_blinker() {
	int ticks_per_interrupt = alt_ticks_per_second() / 4;
	alt_alarm_start(&alarm, ticks_per_interrupt, alarm_blink_isr, NULL);
}

/*
 * @brief Initialize all map tiles
 */
void show_game(void) {
	int x;
	int y;
	int x_coord = MAP_CORNER_X;
	int y_coord = MAP_CORNER_Y;

	// Initialize screen. Clear everything.
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_char_buffer_clear(char_buffer);

	for(x = 0; x < DIMENSION_OF_MAP_X; x++, x_coord += SIZE_OF_TILE) {
		y_coord = MAP_CORNER_Y;
		for(y = 0; y < DIMENSION_OF_MAP_Y; y++, y_coord += SIZE_OF_TILE) {
			map[x][y].pos.x = x_coord;
			map[x][y].pos.y = y_coord;
			map[x][y].coords.x = x;
			map[x][y].coords.y = y;
			map[x][y].type = GRASS;
			map[x][y].occupied_by = NULL;
			map[x][y].distance = 10000;
			draw_sprite(x_coord, y_coord, map[x][y].type);
		}
	}
	randomize_map();
	initialize_blinker();

}

/*
 * @brief Move character sprite to a new tile, erasing and redrawing at a rate of 32 fps.
 * @param dx Distance to move character in x axis for every redraw
 * @param dy Distance to move character in x axis for every redraw
 * @param old_x Previous coordinate of character on map in x axis
 * @param old_y Previous coordinate of character on map in y axis
 */
void animate_to_tile(int ram_location, int dx, int dy, int old_x, int old_y, int new_x, int new_y, int type) {
	int i = 0;
	int ticks_per_mvmnt = alt_timestamp_freq() / 32;

	for(i = 0; i < 16; i++) {
		alt_timestamp_start();
		draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, map[old_x][old_y].type);
		draw_sprite(map[new_x][new_y].pos.x, map[new_x][new_y].pos.y, map[new_x][new_y].type);
		if(i % 8 <= 3) {
			draw_sprite(map[old_x][old_y].pos.x + i * dx, map[old_x][old_y].pos.y + i * dy, ram_location + type);
		}
		else {
			draw_sprite(map[old_x][old_y].pos.x + i * dx, map[old_x][old_y].pos.y + i * dy, ram_location + type - 1);
		}
		draw_corner(old_x, old_x, map[old_x][old_y].type);
		while ((int)alt_timestamp() < ticks_per_mvmnt) {}
	}
	draw_sprite(map[old_x][old_y].pos.x, map[old_x][old_y].pos.y, map[old_x][old_y].type);
}


/*
 * @brief Move character to the selected location.
 * @param ram_location The first animation sprite of character in ram
 * @param old_x Previous coordinate of character on map in x axis
 * @param old_y Previous coordinate of character on map in y axis
 ^ @param new_x The final x destination for the character
 * @param new_y the final y destination for the character
 */
void animate(int ram_location, int old_x, int old_y, int new_x, int new_y) {
	int dist = 1;
	music_choose(MOVE_FX);
	int *path = (int *) calloc(map[new_x][new_y].distance, sizeof(int));
	get_path(new_x, new_y, path);
	while(dist <= map[new_x][new_y].distance) {
		if(path[dist] == 0) {
			animate_to_tile(ram_location, 1, 0, old_x, old_y, old_x + 1, old_y, RIGHT1);
			old_x++;
		}
		else if(path[dist] == 1) {
			animate_to_tile(ram_location, -1, 0, old_x, old_y, old_x - 1, old_y, LEFT1);
			old_x--;
		}
		else if(path[dist] == 2) {
			animate_to_tile(ram_location, 0, 1, old_x, old_y, old_x, old_y + 1, DOWN1);
			old_y++;
		}
		else {
			animate_to_tile(ram_location, 0, -1, old_x, old_y, old_x, old_y - 1, UP1);
			old_y--;
		}
		dist++;
	}
	free(path);
}

/*
 *@brief Gets the path from the selected character to the position.
 *@param new_x The selected x postion
 *@param new_y The selected y position
 *@param path The buffer to place the path 0 = right, 1 = left, 2 = up, 3 = down
 */
void get_path(int new_x, int new_y, int *path) {
	printf("get_path\n");
	int i = map[new_x][new_y].distance;
	while(map[new_x][new_y].distance != 0) {
		printf("x:%d y:%d i:%d   ", new_x, new_y, i);
		if(new_x > 0 && map[new_x][new_y].distance > map[new_x - 1][new_y].distance) {
			new_x--;
			path[i] = 0;
		}
		else if(new_x < DIMENSION_OF_MAP_X - 1 && map[new_x][new_y].distance > map[new_x + 1][new_y].distance) {
			new_x++;
			path[i] = 1;
		}
		else if(new_y > 0 && map[new_x][new_y].distance > map[new_x][new_y - 1].distance) {
			new_y--;
			path[i] = 2;
		}
		else if(new_y < DIMENSION_OF_MAP_Y - 1 && map[new_x][new_y].distance > map[new_x][new_y + 1].distance) {
			new_y++;
			path[i] = 3;
		}
		i--;
	}
	printf("end get path\n");
}

/*
 * @brief Move the character from current position to selected position
 * @param player_id The current player Player id
 * @param character_id The character that is moving
 * @param old_x Current map coordinate of character in x
 * @param old_y Current map coordinate of character in y
 * @param new_x Destination map coordinate of character in x
 * @param new_y Destination map coordinate of character in y
 */
void move_player(int player_id, int char_id, int old_x, int old_y, int new_x, int new_y) {
	map[old_x][old_y].occupied_by = NULL;
	if((new_x == -1) || (new_y == -1) || (new_x == DIMENSION_OF_MAP_X) || (new_y == DIMENSION_OF_MAP_Y)) {
		return;
	}
	animate(char_id * (SPRITES_PER_CHAR - 1) + ANIMATION_HARDWARE, old_x, old_y, new_x, new_y);
	Players[player_id]->characters[char_id].pos.x = new_x;
	Players[player_id]->characters[char_id].pos.y = new_y;
	map[new_x][new_y].occupied_by = &Players[player_id]->characters[char_id];
}

/*
 *@brief Initializes all the player and character data
 */
void initialize_players() {
	int i;
	int j;
	int x;
	int y;

	for(i = 0; i < NO_PLAYERS; i++) {
		Players[i]->characters_remaining = CHARS_PER_PLAYER;
		for(j = 0; j < CHARS_PER_PLAYER; j++) {
			Players[i]->characters[j].hp = classes[j].hp;
			Players[i]->characters[j].atk = classes[j].atk;
			Players[i]->characters[j].def = classes[j].def;
			Players[i]->characters[j].rng = classes[j].rng;
			Players[i]->characters[j].movement = classes[j].movement;
			Players[i]->characters[j].class = classes[j].class;
			Players[i]->characters[j].colour = colours[i][j];
			Players[i]->characters[j].team = i;
			Players[i]->characters[j].id = j;

			Players[i]->characters[j].pos.x = start_pos[i][j][0];
			Players[i]->characters[j].pos.y = start_pos[i][j][1];
			Players[i]->characters[j].move = MOVE;
			Players[i]->characters[j].standing = i * CHARS_PER_PLAYER + NUM_SPRITE_TYPES + j;

			x = Players[i]->characters[j].pos.x;
			y = Players[i]->characters[j].pos.y;
			sprite_init(i, j);
			load_sprite_hardware(i, j, Players[i]->characters[j].standing, STANDING);
			map[x][y].occupied_by = &Players[i]->characters[j];
			draw_sprite(map[x][y].pos.x, map[x][y].pos.y, Players[i]->characters[j].standing);
			draw_healthbar(i, j, healthbar_pos[i][j][0], healthbar_pos[i][j][1]);
		}
	}
}

/*
 *@brief Checks if player can move onto this tile (Used in BFS)
 *@param player_id The current player id
 *@param x The map coordinate being checked in x
 *@param y The map coordinate being checked in y
 *@returns 1 if space is free to move to 0 otherwise
 */
int tile_is_free(int player_id, int x, int y) {
	if ((0 <= x) && (x < DIMENSION_OF_MAP_X ) && (0 <= y) && (y < DIMENSION_OF_MAP_Y) &&
			(map[x][y].occupied_by == NULL) && (map[x][y].explored == 0) && (map[x][y].type == GRASS)) {
		return 1;
	} else {
		return 0;
	}
}

/*
 *@brief Checks if player can attack this tile (Used in BFS)
 *@param player_id The current player id
 *@param x The map coordinate being checked in x
 *@param y The map coordinate being checked in y
 *@returns 1 if space is attackable 0 otherwise
 */
int tile_is_attackable(int player_id, int x, int y) {
	if ((0 <= x) && (x < DIMENSION_OF_MAP_X) && (0 <= y) && (y < DIMENSION_OF_MAP_Y) &&
			(map[x][y].explored == 0) && ((map[x][y].occupied_by->team != player_id) || (map[x][y].occupied_by == NULL)) && map[x][y].type == GRASS) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * @brief Randomly places rocks and a lake in the map, ensuring it does not block characters
 *        in their initial positions.
 */
void randomize_map(void) {
	// Randomly create 1 lake and 1 river
	int src_x = rand() % DIMENSION_OF_MAP_X;
	int src_y = rand() % DIMENSION_OF_MAP_Y;
	int size = rand() % 4;
	int i;
	game_tile** valid_moves = (game_tile**) calloc(75, sizeof(game_tile*));

	// Create random body of water, ensure it doesn't block players in
	if (size == 0) {
		size++;
	}
	if (abs(src_x - DIMENSION_OF_MAP_X) <= 3) {
		if (src_x < 3) {
			src_x = 3;
		} else {
			src_x = 12;
		}
	}
	dfs_map(0, 0, src_x, src_y, size, valid_moves, tile_is_free);
	for (i = 0; valid_moves[i] != 0; i++) {
		valid_moves[i]->type = WATER;
		valid_moves[i]->distance = 10000;
		valid_moves[i]->explored = 0;
		draw_sprite(valid_moves[i]->pos.x, valid_moves[i]->pos.y, WATER);
	}

	// Randomly disperse rocks throughout game
	size = (rand() % 10) + 10;
	for (i = 0; i < size; i++) {
		src_x = rand() % (DIMENSION_OF_MAP_X - 4);
		src_y = rand() % DIMENSION_OF_MAP_Y;
		map[src_x + 2][src_y].type = ROCK;
		draw_sprite(map[src_x + 2][src_y].pos.x, map[src_x + 2][src_y].pos.y, ROCK);
	}
	draw_map();
	free(valid_moves);
}

/*
 * @brief Check if a specific tile is in a list of moves
 * @param x The x coordinate of the map tile to look for
 * @param y The y coordinate of the map tile to look for
 * @param moves A list of pointers to map tiles
 * @return 1 if the tile is found in moves, 0 if the tile is not found in moves
 */
int is_valid_move(int x, int y, game_tile** moves) {
	int i;

	for(i = 0; moves[i] != 0; i++) {
		if (&map[x][y] == moves[i]) {
			return 1;
		}
	}
	return 0;
}

/*
 * @brief Redraw a characters health bar
 * @param player_id Player id
 * @param character_id Character id
 */
void update_healthbar(int player_id, int character_id) {
	// Black out health lost
	int pixel_per_hp = HEALTHBAR_LEN / classes[Players[player_id]->characters[character_id].class].hp;
	int pixel_health = Players[player_id]->characters[character_id].hp * pixel_per_hp;

	alt_up_pixel_buffer_dma_draw_box(pixel_buffer,
			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8,
			healthbar_pos[player_id][character_id][1] + 1,

			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8 + HEALTHBAR_LEN,
			healthbar_pos[player_id][character_id][1] + SIZE_OF_TILE/2 - 1, 0x0, 0);
	if(pixel_health <= 0) {
		return;
	}
	alt_up_pixel_buffer_dma_draw_box(pixel_buffer,
			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8,
			healthbar_pos[player_id][character_id][1] + 1,
			healthbar_pos[player_id][character_id][0] + SIZE_OF_TILE/2 + 8 + pixel_health,
			healthbar_pos[player_id][character_id][1] + SIZE_OF_TILE/2 - 1, 0xF822, 0);
}

/*
 * @brief Update player and character with appropriate information for a character that
 *        has just died
 * @param player_id Player id
 * @param character_id Character id
 */
void character_is_dead(int player_id, int character_id) {
	// Remove character from map
	music_choose(UNIT_DIE_FX);
	move_player(player_id, character_id, Players[player_id]->characters[character_id].pos.x,
			Players[player_id]->characters[character_id].pos.y, -1, -1);
	Players[player_id]->characters_remaining--;
}

/*
 * @brief Attack a specific location.
 * @param player_id Player id
 * @param character_id Character id
 * @param x The x coordinate of the location to attack
 * @param y The y coordinate of the location to attack
 */
void attack_player(int player_id, int character_id, int x, int y) {
	music_choose((sound_fx)(Players[player_id]->characters[character_id].class));
	if(((x == Players[player_id]->characters[character_id].pos.x) &&
			(y == Players[player_id]->characters[character_id].pos.y)) || map[x][y].occupied_by == NULL) {
		// If selecting self, it means player chose not to attack
		return;
	}
	map[x][y].occupied_by->hp -= Players[player_id]->characters[character_id].atk;
	update_healthbar(map[x][y].occupied_by->team, map[x][y].occupied_by->id);
	printf("Player %d character %d attacked player %d and caused %d damage.\n",
			player_id, character_id, map[x][y].occupied_by->team, Players[player_id]->characters[character_id].atk);
	if(map[x][y].occupied_by->hp <= 0) {
			// Cannot have negative health
			map[x][y].occupied_by->hp = 0;
			character_is_dead(map[x][y].occupied_by->team, map[x][y].occupied_by->id);
	}
}

/*
 *@brief Move the cursor based on the keypress
 *@param move The keypress to determine direction
 *@param sel_x The current x position of cursor
 *@param sel_y The current y position of cursor
 */
void move_cursor(keypress move, int *sel_x, int *sel_y) {
	if (move == UP && *sel_y > 0) {
			*sel_y -= 1;
			draw_cursor(*sel_x, *sel_y + 1, *sel_x, *sel_y, 0xF81F);
	} else if (move == DOWN && *sel_y < DIMENSION_OF_MAP_Y - 1) {
			*sel_y += 1;
			draw_cursor(*sel_x, *sel_y - 1, *sel_x, *sel_y, 0xF81F);
	} else if (move == LEFT && *sel_x > 0) {
			*sel_x -= 1;
			draw_cursor(*sel_x + 1, *sel_y, *sel_x, *sel_y, 0xF81F);
	} else if (move == RIGHT && *sel_x < DIMENSION_OF_MAP_X - 1) {
			*sel_x += 1;
			draw_cursor(*sel_x - 1, *sel_y, *sel_x, *sel_y, 0xF81F);
	}
}

/*
 *@brief The space to select for movement or attacks
 *@param player_id The current player id to determine input device
 *@param valid_moves The valid move or attack spaces
 *@param sel_x The current map x coordinate of the cursor
 *@param sel_y The current map y coordinate of the cursor
 *@returns 1 if space is selected 0 if player wants to deselect
 */
int select_space(int player_id, game_tile **valid_moves, int *sel_x, int *sel_y) {
	keypress move = get_player_input(player_id);
	draw_cursor(*sel_x, *sel_y, *sel_x, *sel_y, 0xF81F);
	while(1) {
		move_cursor(move, sel_x, sel_y);
		if (move == ENTER && is_valid_move(*sel_x, *sel_y, valid_moves)) {
			return 1;
		} else if (move == ESC) {
			return 0;
		}
		move = get_player_input(player_id);
	}
}

/*
 *@brief Do the movement phase for the selected character
 *@param player_id The current players turn
 *@param character_id The selected character to do movement
 */
void do_movement(int player_id, int character_id) {
	printf("do movement\n");
	int i;
	int old_x = Players[player_id]->characters[character_id].pos.x;
	int old_y = Players[player_id]->characters[character_id].pos.y;
	int new_x = old_x;
	int new_y = old_y;
	game_tile** valid_moves;
	valid_moves = (game_tile**) calloc(75, sizeof(game_tile*));
	dfs_map(player_id, character_id, old_x, old_y, Players[player_id]->characters[character_id].movement,valid_moves, &tile_is_free);
	for(i = 0; valid_moves[i] != 0; i++) {
		map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type |= 0x100;
		draw_sprite(map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].pos.x, map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].pos.y, map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type);
	}
	draw_characters();

	if (select_space(player_id, valid_moves, &new_x, &new_y)) {
		move_player(player_id, character_id, old_x, old_y, new_x, new_y);
		Players[player_id]->characters[character_id].move = ATTACK;
	}

	for(i = 0; valid_moves[i] != 0; i++) {
		map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type &= 0x0FF;
		draw_sprite(map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].pos.x, map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].pos.y, map[valid_moves[i]->coords.x][valid_moves[i]->coords.y].type);
		valid_moves[i]->explored = 0;
		valid_moves[i]->distance = 10000;
	}
	draw_characters();
	free(valid_moves);
}

/*
 *@brief Do the attack phase for the selected character
 *@param player_id The current players turn
 *@param character_id The selected character to do attack
 */
void do_attack(int player_id, int character_id) {
	int i = 0;
	int old_x = Players[player_id]->characters[character_id].pos.x;
	int old_y = Players[player_id]->characters[character_id].pos.y;
	int new_x = old_x;
	int new_y = old_y;
	game_tile** valid_attacks;
	valid_attacks = (game_tile**) calloc(75, sizeof(game_tile*));
	dfs_map(player_id, character_id, old_x, old_y, Players[player_id]->characters[character_id].rng,valid_attacks, &tile_is_attackable);
	for(i = 0; valid_attacks[i] != 0; i++) {
		map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type |= 0x200;
		draw_sprite(map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].pos.x, map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].pos.y, map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type);
	}
	draw_characters();
	if (select_space(player_id, valid_attacks, &new_x, &new_y)) {
		attack_player(player_id, character_id, new_x, new_y);
		Players[player_id]->characters[character_id].move = DONE;
	}
	for(i = 0; valid_attacks[i] != 0; i++) {
		map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type &= 0x0FF;
		draw_sprite(map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].pos.x, map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].pos.y, map[valid_attacks[i]->coords.x][valid_attacks[i]->coords.y].type);
		valid_attacks[i]->explored = 0;
		valid_attacks[i]->distance = 10000;
	}
	draw_characters();
	free(valid_attacks);
}

/*
 *@brief Select the character to do an action (movement or attack)
 *@param player_id The current players turn
 *@returns 1 if an action is taken or 0 if player wants to quit the game
 */
int select_character(int player_id) {
	int sel_x = 0, sel_y = 0;
	int character_id = 0, old_character_id;
	character_option curr_move = find_player_move(player_id);//Players[0]->characters[0].move;
	keypress move = get_player_input(player_id);

	while(1) {
		//move_cursor(move, &sel_x, &sel_y);
		if(move == ENTER && map[sel_x][sel_y].occupied_by != NULL) {
			if(map[sel_x][sel_y].occupied_by->team == player_id && map[sel_x][sel_y].occupied_by->move == MOVE) {
				do_movement(player_id, map[sel_x][sel_y].occupied_by->id);
				return 1;
			}
			else if(map[sel_x][sel_y].occupied_by->move == ATTACK) {
				do_attack(player_id, map[sel_x][sel_y].occupied_by->id);
				return 1;
			}
		} else if(move == ESC) {
			if (!draw_exit_screen(player_id)) {
				return 0;
			}
		} else if (move == NEXT) {
			for (character_id = 0; character_id < CHARS_PER_PLAYER; character_id++) {
				if ((Players[player_id]->characters[character_id].move == curr_move) &&
						(Players[player_id]->characters[character_id].move != DONE)) {
					Players[player_id]->characters[character_id].move++;
				}
			}
			draw_sprite(map[sel_x][sel_y].pos.x, map[sel_x][sel_y].pos.y, map[sel_x][sel_y].type);
			draw_characters();
			return 1;
		} else if (move == LEFT) {
			old_character_id = character_id;
			do {
				if(character_id > 0) {
					character_id--;
				} else {
					character_id = CHARS_PER_PLAYER - 1;
				}
			} while(Players[player_id]->characters[character_id].hp == 0);
			draw_cursor(Players[player_id]->characters[old_character_id].pos.x,
						Players[player_id]->characters[old_character_id].pos.y,
						Players[player_id]->characters[character_id].pos.x,
						Players[player_id]->characters[character_id].pos.y, 0xF81F);
			sel_x = Players[player_id]->characters[character_id].pos.x;
			sel_y = Players[player_id]->characters[character_id].pos.y;
		} else if (move == RIGHT) {
			old_character_id = character_id;
			do {
				if(character_id < CHARS_PER_PLAYER - 1) {
					character_id++;
				} else {
					character_id = 0;
				}
			} while(Players[player_id]->characters[character_id].hp == 0);
			draw_cursor(Players[player_id]->characters[old_character_id].pos.x,
						Players[player_id]->characters[old_character_id].pos.y,
						Players[player_id]->characters[character_id].pos.x,
						Players[player_id]->characters[character_id].pos.y, 0xF81F);
			sel_x = Players[player_id]->characters[character_id].pos.x;
			sel_y = Players[player_id]->characters[character_id].pos.y;
		}
		move = get_player_input(player_id);
	}
}

/*
 * @param player_id
 * @returns 1 if players turn is not done, 0 if players turn is done
 */
int is_turn_done(int player_id) {
	int i = 0;
		for(i = 0; i < CHARS_PER_PLAYER; i++) {
			if(Players[player_id]->characters[i].move != DONE && Players[player_id]->characters[i].hp > 0) {
				return 1;
			}
		}
		return 0;
}

/*
 *@brief Resets characters to the movement phase for next turn
 *@param player_id the current players turn.
 */
void reset_turn(int player_id) {
	int i = 0;
	for(i = 0; i < CHARS_PER_PLAYER; i++) {
		Players[player_id]->characters[i].move = MOVE;
	}
}

/*
 * @brief The main function of the Game screen.
 */
void play_game() {
	int i = 0;
	int j =0;
	main_player_id = 0;

	hardware_init();
	show_game();
	initialize_players();
	draw_map();
	load_turn(main_player_id);


	 while(1){
		 while(is_turn_done(main_player_id)) {
			 if (Players[main_player_id]->characters_remaining == 0) {
				 printf("Game over. Player %d lost!\n", main_player_id);
				 alt_alarm_stop(&alarm);
				 return;
			 } else if (!select_character(main_player_id)) {
				 alt_alarm_stop(&alarm);
				 return;
			 }
		 }
		 reset_turn(main_player_id);
		 main_player_id = !main_player_id;
		 load_turn(main_player_id);
	}
	alt_alarm_stop(&alarm);
	return;
}
