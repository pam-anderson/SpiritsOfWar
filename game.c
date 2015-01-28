#include <stdio.h>
#include "SoW.h"

int cursor;


struct Character
{
	int HP;
	int NumMoves;
	int Range;
	int Str;
	int Def;
}Unit1;

struct MapTile
{
	int Status;
}Map[8][8];


enum MenuOptions
{
	GameStart,
	Help	
};

void gamemenu(void)
{
	char c=0;

	cursor = GameStart;
	keyboard_read();
	printf("Cursor is now at GameStart!\n");
	
	while(1)
	{
		if(c == 87) // Replace with Up Arrow
		{
			if(cursor == GameStart){} // Do Nothing
			else if(cursor == Help) // Go Down
			{
				cursor = GameStart;
				printf("Cursor is now at GameStart!\n");
			}
			else
				cursor--; // Go Down if not Edge Case
		}

		else if(c == 83)// Replace with Down Arrow
		{
			printf("!!!!\n");
			if(cursor == GameStart)
			{
				cursor = Help;
				printf("Cursor is now at Help!\n");
			}
			else if(cursor == Help){}
			else
				cursor++; // Go Up if not Edge Case

		}

		else if(c == ' ')
		{
			break;
		}

		printf("%i %i\n", c, cursor);


	}

}	


/*int main()
{
	keyboard_init();
	gamemenu();
	printf("Cursor is at %i", cursor);
    return 0;
}*/

