#include "SoW.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>

game_tile map[8][8];
player Players[2];
int CurrentPlayer = 1;
int CurrentCharacter = 0;

#define DEFAULT_HP 		10
#define DEFAULT_ATTACK	3
#define DEFAULT_DEFENSE	5
#define NO_MOVES 2
#define ATK_RNG 1
#define UP	87
#define DOWN 83
#define LEFT 65
#define RIGHT 68


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

void initialize_players() {

	int i,j;

	Players[0]->characters[0].pos.x = 0;
	Players[0]->characters[0].pos.y = 7;           //        [ ][ ] [ ][ ] [ ][ ] [0][1]
	                                               //        [ ][ ] [ ][ ] [ ][ ] [ ][2]
	                                               //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[1].pos.x = 0;	       //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[1].pos.y = 7;	       //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	                                    	       //        [ ][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[2].pos.x = 1;           //        [0][ ] [ ][ ] [ ][ ] [ ][ ]
	Players[0]->characters[2].pos.y = 6;		   //        [1][2] [ ][ ] [ ][ ] [ ][ ]

	Players[1]->characters[0].pos.x = 6;
	Players[1]->characters[0].pos.y = 0;

	Players[1]->characters[1].pos.x = 7;
	Players[1]->characters[1].pos.y = 0;

	Players[1]->characters[2].pos.x = 7;
	Players[1]->characters[2].pos.y = 1;

	for(i = 0; i<NO_PLAYERS; i++)
	{
		for(j = 0; j<CHARS_PER_PLAYER; j++)
		{
			Players[i]->characters[j].hp = DEFAULT_HP;
			Players[i]->characters[j].atk = DEFAULT_ATTACK;
			Players[i]->characters[j].def = DEFAULT_DEFENSE;
		}
	}
}

void attack_menu()
{

		int	cursorx = Players[CurrentPlayer]->characters[CurrentCharacter].pos.x;
		int	cursory = Players[CurrentPlayer]->characters[CurrentCharacter].pos.y;

		int i;
		int enemy = CurrentPlayer ^ 1;

		while(1)
		{
			if(CurrentPlayer == 0)
				keyboard_read();
			else if(CurrentPlayer == 1){// Read from Serial }
			}


			if(*KeyInput == UP )
			{
				if(cursory == 0 || cursory-ATK_RNG < Players[CurrentPlayer]->characters[CurrentCharacter].pos.y-ATK_RNG ){}
				else
					cursory--;
			}
			else if(*KeyInput == DOWN )
			{
				if(cursory == 7 || cursory+ATK_RNG > Players[CurrentPlayer]->characters[CurrentCharacter].pos.y + ATK_RNG){}
				else
					cursory++;
			}
			else if(*KeyInput == LEFT )
			{
				if(cursorx == 0 || cursorx-ATK_RNG < Players[CurrentPlayer]->characters[CurrentCharacter].pos.x - ATK_RNG){}
				else
					cursorx--;
			}
			else if(*KeyInput == RIGHT)
			{
				if(cursorx == 7 || cursorx + ATK_RNG > Players[CurrentPlayer]->characters[CurrentCharacter].pos.x + ATK_RNG ){}
				else
					cursorx++;
			}
			else if(*KeyInput == ' ')
			{
				break;
			}
		}

		for(i = 0; i<CHARS_PER_PLAYER; i++)
		{
			if(Players[enemy]->characters[i].pos.x == cursorx && Players[enemy]->characters[i].pos.y == cursory)
			{
				Players[enemy]->characters[i].hp = Players[enemy]->characters[i].hp - (Players[CurrentPlayer]->characters[CurrentCharacter].atk - Players[enemy]->characters[i].def) ;
			}
		}


}

void move_menu()
{

	// Remove Menu add Valid Move Highlights?
	int cursorx = Players[CurrentPlayer]->characters[CurrentCharacter].pos.x;
	int cursory = Players[CurrentPlayer]->characters[CurrentCharacter].pos.y;

	while(1)
	{
		if(CurrentPlayer == 0)
			keyboard_read();
		else if(CurrentPlayer == 1){// Read from Serial }
		}


		if(*KeyInput == UP )
		{
			if(cursory == 0 || cursory-1 < Players[CurrentPlayer]->characters[CurrentCharacter].pos.y-NO_MOVES){}
			else
				cursory--;
		}
		else if(*KeyInput == DOWN )
		{
			if(cursory == 7 || cursory+1 > Players[CurrentPlayer]->characters[CurrentCharacter].pos.y + NO_MOVES ){}
			else
				cursory++;
		}
		else if(*KeyInput == LEFT )
		{
			if(cursorx == 0 || cursorx-1 < Players[CurrentPlayer]->characters[CurrentCharacter].pos.x - NO_MOVES){}
			else
				cursorx--;
		}
		else if(*KeyInput == RIGHT)
		{
			if(cursorx == 7 || cursorx + 1 > Players[CurrentPlayer]->characters[CurrentCharacter].pos.x + NO_MOVES ){}
			else
				cursorx++;
		}
		else if(*KeyInput == ' ')
		{
			break;
		}
	}

	Players[CurrentPlayer]->characters[CurrentCharacter].pos.x = cursorx;
	Players[CurrentPlayer]->characters[CurrentCharacter].pos.y = cursory;


}

void play_game() {
	int i, TurnDone, GameOver, cursor;
	show_game();
	initialize_players();
	// draw map
	while(alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC) == -1){}

	while(*SERIAL_DATA_LOC != 27 || GameOver) // While ESC has not been pressed.
	{
		CurrentPlayer = CurrentPlayer ^ 1;
		TurnDone = 0;

		for(CurrentCharacter = 0; CurrentCharacter<CHARS_PER_PLAYER; CurrentCharacter++)
		{
			if(Players[CurrentPlayer]->characters[i].hp <= 0){}
			else
			{
				// Display Menu Here for Move, Attack, Skip...

				cursor = ATTACK;
				if(CurrentPlayer == 0)
					keyboard_read();
				else if (CurrentPlayer == 1)
				{
					// Read from Serial Instead...
					// Maybe *KeyInput = *SERIAL_DATA_LOC
				}

				while(1)
				{
					if(*KeyInput == UP)
					{
						if(cursor == ATTACK){}
						else
							cursor++;

					}

					else if (*KeyInput == DOWN)
					{
						if(cursor == SKIP){}
						else
							cursor--;
					}

					else if (*KeyInput == ' ')
						break;

				}

				if( cursor == ATTACK)
					attack_menu();
				else if(cursor == MOVE)
					move_menu();
				// Do nothing if Skip



			}



		}
	}

	return;

}





/*typedef struct {
int x;
int y;
} position;

typedef struct {
position pos;
} game_tile;

typedef struct {
position pos;
int hp;
const int atk;
const int def;
} character;

typedef struct {
character characters[CHARS_PER_PLAYER];
} player;*/


