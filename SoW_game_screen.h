#define MAX_SPACES_MOVE 3
#define NO_MOVES        2
#define ATK_RNG         1
#define HEALTHBAR_LEN   50
#define SERIAL          0
#define KEYBOARD        1
#define NUM_OF_CLASSES  3

#define WARRIOR_HP 		15
#define WARRIOR_ATTACK	5
#define WARRIOR_DEFENSE	6
#define WARRIOR_RANGE   1
#define WARRIOR_MOVEMENT 2

#define RANGER_HP 		12
#define RANGER_ATTACK	4
#define RANGER_DEFENSE	4
#define RANGER_RANGE    2
#define RANGER_MOVEMENT 5

#define MAGE_HP 		10
#define MAGE_ATTACK	    4
#define MAGE_DEFENSE	2
#define MAGE_RANGE      3
#define MAGE_MOVEMENT	3

/* MAP DEFINITIONS */
#define SIZE_OF_TILE 	16
#define SIZE_OF_MAP 	128
#define DIMENSION_OF_MAP_X 16
#define DIMENSION_OF_MAP_Y 10
#define MAP_CORNER_X 	32
#define MAP_CORNER_Y 	40
#define CHARS_PER_PLAYER 3
#define NO_PLAYERS 		 2

#define NUM_SPRITE_TYPES 3
#define SPRITES_PER_CHAR 9
#define ANIMATION_HARDWARE 9
#define STANDING		0
#define UP1				1
#define UP2				2
#define RIGHT1			3
#define RIGHT2			4
#define DOWN1			5
#define DOWN2			6
#define LEFT1			7
#define LEFT2			8

typedef enum {
	GRASS,
	WATER,
	ROCK
} sprite;

typedef enum {
    MOVE,
    ATTACK,
    DONE
} character_option;

typedef enum {
	WARRIOR,
	RANGER,
	MAGE
} class;

typedef struct {
	class class;
	int hp;
	int atk;
	int def;
	int rng;
	int movement;
}class_defaults;

typedef struct {
	int x;
	int y;
} position;

typedef struct {
	position pos;
	class class;
	int hp;
	int atk;
	int def;
	int rng;
	int colour;
	int team; // Player ID
	int id;   // Character ID
	int movement;
	character_option move;
	int **sprites;
	int standing;
} character;

typedef struct {
	position coords;
	position pos;
	sprite type;
	character* occupied_by;
	int explored; // Used in DFS
	int distance; //Needed for Path Finding
} game_tile;

typedef struct {
	character characters[CHARS_PER_PLAYER];
	int characters_remaining;
} player[2];

/* Map tiles */
game_tile map[DIMENSION_OF_MAP_X][DIMENSION_OF_MAP_Y];

/* Player */
player Players[NO_PLAYERS];

/* Start grids of characters */
int start_pos[NO_PLAYERS][CHARS_PER_PLAYER][2] = {{{0, DIMENSION_OF_MAP_Y - 2}, {0, DIMENSION_OF_MAP_Y - 1},
		{1, DIMENSION_OF_MAP_Y - 1}}, {{DIMENSION_OF_MAP_X - 2, 0}, {DIMENSION_OF_MAP_X - 1, 0},
		{DIMENSION_OF_MAP_X - 1, 1}}};

/* Coordinates of top left corner of health bar, and current highlighting colour */
static int healthbar_pos[NO_PLAYERS][CHARS_PER_PLAYER][2] = {{{29, 15}, {123, 15}, {217, 15}},
		{{29, 214}, {123, 214}, {217, 214}}};

/* Character class default stats */
static class_defaults classes[NUM_OF_CLASSES] = {
		{WARRIOR, WARRIOR_HP, WARRIOR_ATTACK, WARRIOR_DEFENSE, WARRIOR_RANGE, WARRIOR_MOVEMENT},
		{RANGER, RANGER_HP, RANGER_ATTACK, RANGER_DEFENSE, RANGER_RANGE, RANGER_MOVEMENT},
		{MAGE, MAGE_HP, MAGE_ATTACK, MAGE_DEFENSE, MAGE_RANGE, MAGE_MOVEMENT} };

int colours[NO_PLAYERS][CHARS_PER_PLAYER] = {{0xF808, 0x7E0, 0x1F}, {0xE700, 0xE70, 0xE7}};
char blinker = 0xFFFF;
int main_player_id = 0;

int is_turn_done(int);


int grass[] = {0x1d20, 0x1ce0, 0x1d00, 0x1d00, 0x1cc0, 0x1d00, 0x1d00, 0x1cc0, 0x1ca0, 0x1cc0, 0x1ce0, 0x1d00, 0x1ce0, 0x1d00, 0x1c60, 0x1d00,
	 0x1c80, 0x14c0, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1ce0, 0x14e0, 0x14c0, 0x1d00, 0x1cc0, 0x1ca0, 0x1cc0, 0x1ca0, 0x1c80,
	 0x1cc0, 0x1ce0, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1ce0, 0x1ce0, 0x1cc0, 0x1ce0, 0x1cc0, 0x1ca0, 0x1cc0, 0x1cc0, 0x1ca0,
	 0x1d20, 0x1d20, 0x1d20, 0x1d00, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d00, 0x1d00, 0x1cc0, 0x1cc0, 0x1ca0, 0x1ca0, 0x1cc0, 0x1cc0, 0x1ce0,
	 0x1d00, 0x1d00, 0x1d20, 0x1d00, 0x1ce0, 0x1ce0, 0x1d00, 0x1d00, 0x1d20, 0x1ce0, 0x1ca0, 0x1c80, 0x1ca0, 0x1ca0, 0x1cc0, 0x1d00,
	 0x1ce0, 0x1cc0, 0x1ce0, 0x1ce0, 0x1cc0, 0x1ce0, 0x1d00, 0x1d20, 0x1d00, 0x1ce0, 0x1c80, 0x1c60, 0x1ca0, 0x1ca0, 0x1ca0, 0x1d00,
	 0x1cc0, 0x1cc0, 0x1cc0, 0x1ca0, 0x1ca0, 0x1cc0, 0x1ce0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1ca0, 0x1ce0, 0x1d00, 0x1d20, 0x1ca0, 0x1ce0,
	 0x1cc0, 0x1ca0, 0x1cc0, 0x1ca0, 0x1ca0, 0x1d00, 0x1d00, 0x1cc0, 0x1ca0, 0x1cc0, 0x1ce0, 0x1d20, 0x1d40, 0x1d40, 0x1cc0, 0x1d20,
	 0x1ce0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1ce0, 0x1d20, 0x1d40, 0x1d60, 0x1ce1, 0x1d00,
	 0x1d40, 0x1d20, 0x1d00, 0x1cc0, 0x1d00, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d20, 0x1d40, 0x1d01, 0x1ca0,
	 0x1d40, 0x1d00, 0x1d40, 0x1cc0, 0x1cc0, 0x1d00, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d00, 0x1ce0, 0x1ce0, 0x1ca0,
	 0x1d00, 0x14a0, 0x1ce0, 0x1c80, 0x1c80, 0x1ce0, 0x1d00, 0x1d00, 0x1d00, 0x1d00, 0x1ce0, 0x1ce0, 0x1ce0, 0x1d00, 0x1ce0, 0x1c80,
	 0x14a0, 0x1cc0, 0x1ca0, 0x1c80, 0x1cc0, 0x14a0, 0x14c0, 0x1d00, 0x1d00, 0x1ce0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d00, 0x14a0,
	 0x1500, 0x1cc0, 0x1cc0, 0x1cc0, 0x1cc0, 0x14c0, 0x14c0, 0x1d00, 0x1ce0, 0x1ca0, 0x1cc0, 0x1ca0, 0x1cc0, 0x1ce0, 0x1ce0, 0x1500,
	 0x1500, 0x1500, 0x1ce0, 0x1cc0, 0x1ca0, 0x14a0, 0x1ca0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1cc0, 0x1cc0, 0x14e0, 0x14e0, 0x1500,
	 0x1d20, 0x1d00, 0x1ce0, 0x1ca0, 0x1ca0, 0x1cc0, 0x1ce0, 0x1cc0, 0x1ca0, 0x1cc0, 0x1cc0, 0x1ce0, 0x1d00, 0x1d20, 0x1ca0, 0x1d20};

int water[] = {0x353, 0x373, 0x3b5, 0x3b5, 0x3b5, 0x394, 0x394, 0x3b4, 0x3b5, 0x3b5, 0x394, 0x394, 0x394, 0x3d5, 0x3d6, 0x3f6,
		0x373, 0x374, 0x394, 0x394, 0x394, 0x373, 0x373, 0x394, 0x394, 0x394, 0x394, 0x3b4, 0x3d5, 0x3d6, 0x3b5, 0x394,
		0x3b5, 0x3b4, 0x3b4, 0x394, 0x373, 0x373, 0x353, 0x373, 0x394, 0x394, 0x394, 0x3b5, 0x3d5, 0x3d5, 0x394, 0x373,
		0x3d6, 0x3d5, 0x3b5, 0x394, 0x373, 0x373, 0x373, 0x374, 0x394, 0x3b5, 0x3b4, 0x3b5, 0x3d6, 0x3d5, 0x394, 0x373,
		0x417, 0x3d6, 0x3b5, 0x394, 0x394, 0x394, 0x394, 0x394, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3d5, 0x3d6, 0x3b5, 0x373,
		0x3f7, 0x3f7, 0x3d6, 0x3d5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3d5, 0x3b5, 0x394, 0x394, 0x394, 0x3b5, 0x3b5, 0x394,
		0x3b5, 0x3f6, 0x3d6, 0x394, 0x394, 0x394, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x394, 0x373, 0x373, 0x394, 0x394, 0x374,
		0x3b5, 0x3d5, 0x394, 0x373, 0x373, 0x373, 0x394, 0x3b5, 0x3b5, 0x3b5, 0x394, 0x373, 0x373, 0x394, 0x394, 0x353,
		0x3b5, 0x3d5, 0x394, 0x353, 0x352, 0x373, 0x394, 0x394, 0x3b5, 0x3d6, 0x3b5, 0x394, 0x394, 0x3b4, 0x394, 0x373,
		0x3d6, 0x3d5, 0x394, 0x373, 0x373, 0x373, 0x3b4, 0x3b5, 0x3b5, 0x3d6, 0x3d5, 0x3b5, 0x3b5, 0x3d6, 0x3d5, 0x394,
		0x3f6, 0x3f6, 0x3b5, 0x394, 0x394, 0x3b4, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3d5, 0x3f6, 0x3d6, 0x3b5,
		0x3d6, 0x3d6, 0x3d6, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x394, 0x373, 0x374, 0x3b5, 0x3d6, 0x3d6, 0x3d5,
		0x394, 0x3d6, 0x3d6, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x3b5, 0x394, 0x353, 0x373, 0x394, 0x3d5, 0x3d6, 0x3d5,
		0x394, 0x3d5, 0x3d6, 0x3d5, 0x3b5, 0x394, 0x394, 0x3b5, 0x3b5, 0x394, 0x374, 0x394, 0x3b5, 0x3d5, 0x3b5, 0x3b5,
		0x3b5, 0x3d6, 0x3d5, 0x3b5, 0x3b4, 0x394, 0x394, 0x3b4, 0x3b5, 0x3b4, 0x394, 0x3b5, 0x3d5, 0x3b5, 0x394, 0x394,
		0x3f6, 0x3b5, 0x394, 0x373, 0x373, 0x394, 0x3b5, 0x3b5, 0x3b5, 0x3b4, 0x3b4, 0x3b5, 0x3d5, 0x3b5, 0x3b4, 0x394};

int rock[] = { 0x6920, 0x60e0, 0x60c0, 0x68e0, 0x70e0, 0x7100, 0x7900, 0x7100, 0x68e0, 0x58c0, 0x58c0, 0x58c0, 0x60e0, 0x60e0, 0x58e0, 0x6920,
		0x60e0, 0x60e0, 0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x60c0, 0x60e0, 0x68e0, 0x60e0, 0x58c0, 0x6100,
		0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x60c0, 0x60c0, 0x60c0, 0x58c0, 0x60c0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x60c0,
		0x68e0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x58c0, 0x60c0, 0x60c0, 0x60c0, 0x60c0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x58a0,
		0x60e0, 0x60e0, 0x60e0, 0x60c0, 0x58c0, 0x58c0, 0x58c0, 0x60c0, 0x68e0, 0x68e0, 0x60c0, 0x60c0, 0x60c0, 0x60c0, 0x58c0, 0x58c0,
		0x58c0, 0x58c0, 0x58c0, 0x50c0, 0x50a0, 0x50c0, 0x58c0, 0x60e0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60e0,
		0x50c0, 0x50a0, 0x50a0, 0x50a0, 0x50a0, 0x50a0, 0x58c0, 0x60c0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x58c0, 0x60c0, 0x60c0, 0x60e0,
		0x50e0, 0x50c0, 0x58c0, 0x50c0, 0x58c0, 0x58c0, 0x60c0, 0x60c0, 0x60e0, 0x60e0, 0x60c0, 0x60c0, 0x58c0, 0x60c0, 0x60c0, 0x68e0,
		0x58c0, 0x58c0, 0x60c0, 0x60c0, 0x60c0, 0x60e0, 0x68e0, 0x68e0, 0x60c0, 0x60e0, 0x68e0, 0x68e0, 0x60e0, 0x60c0, 0x60c0, 0x68e0,
		0x58c0, 0x68e0, 0x68e0, 0x68e0, 0x60e0, 0x68e0, 0x70e0, 0x68e0, 0x60c0, 0x60c0, 0x68e0, 0x68e0, 0x68e0, 0x60c0, 0x60e0, 0x68e0,
		0x58c0, 0x68e0, 0x70e0, 0x68e0, 0x60e0, 0x60e0, 0x68e0, 0x60e0, 0x58c0, 0x60c0, 0x60e0, 0x68e0, 0x60e0, 0x60c0, 0x60c0, 0x68e0,
		0x58c0, 0x60e0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60c0, 0x60c0, 0x60c0, 0x58c0, 0x60c0, 0x60e0,
		0x58e0, 0x58c0, 0x60c0, 0x60c0, 0x58c0, 0x58c0, 0x58a0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60c0, 0x60e0, 0x68e0,
		0x50e0, 0x50a0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60c0, 0x60e0, 0x60e0, 0x60c0, 0x58c0, 0x58c0, 0x60e0, 0x68e0, 0x68e0,
		0x58e0, 0x48a0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60e0, 0x68e0, 0x68e0, 0x60c0, 0x58c0, 0x58c0, 0x60e0, 0x68e0, 0x68e0,
		0x6940, 0x58e0, 0x58c0, 0x58c0, 0x58c0, 0x58c0, 0x60e0, 0x68e0, 0x68e0, 0x68e0, 0x60e0, 0x58e0, 0x58c0, 0x60c0, 0x60e0, 0x60c0};


char **filenames[6][9] = {{"p0c0s0.bmp", "p0c0s1.bmp", "p0c0s2.bmp", "p0c0s3.bmp", "p0c0s4.bmp", "p0c0s5.bmp", "p0c0s6.bmp", "p0c0s7.bmp", "p0c0s8.bmp"},
			{"p0c1s0.bmp", "p0c1s1.bmp", "p0c1s2.bmp", "p0c1s3.bmp", "p0c1s4.bmp", "p0c1s5.bmp", "p0c1s6.bmp", "p0c1s7.bmp", "p0c1s8.bmp"},
			{"p0c2s0.bmp", "p0c2s1.bmp", "p0c2s2.bmp", "p0c2s3.bmp", "p0c2s4.bmp", "p0c2s5.bmp", "p0c2s6.bmp", "p0c2s7.bmp", "p0c2s8.bmp"},
			{"p1c0s0.bmp", "p1c0s1.bmp", "p1c0s2.bmp", "p1c0s3.bmp", "p1c0s4.bmp", "p1c0s5.bmp", "p1c0s6.bmp", "p1c0s7.bmp", "p1c0s8.bmp"},
			{"p1c1s0.bmp", "p1c1s1.bmp", "p1c1s2.bmp", "p1c1s3.bmp", "p1c1s4.bmp", "p1c1s5.bmp", "p1c1s6.bmp", "p1c1s7.bmp", "p1c1s8.bmp"},
			{"p1c2s0.bmp", "p1c2s1.bmp", "p1c2s2.bmp", "p1c2s3.bmp", "p1c2s4.bmp", "p1c2s5.bmp", "p1c2s6.bmp", "p1c2s7.bmp", "p1c2s8.bmp"}};

