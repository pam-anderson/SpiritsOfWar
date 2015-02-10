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
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ENTER,
	ESC
} keypress;

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

void create_water();


