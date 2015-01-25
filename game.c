#include <stdio.h>
#include "SoW.h"

int cursor;


struct Character
{
	int HP = 10;
	int NumMoves = 3;
	int Range = 1;
	int Str = 2;
	int Def = 1;
}Unit;

struct MapTile
{
	int Status = 0;
}Map[8][8];


enum MenuOptions
{
	GameStart,
	Help	
};

void gamemenu(void)
{
	cursor = GameStart;
	
	while(1)
	{
		keyboard_read();
		if(*KeyInput == 'w') // Replace with Up Arrow
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

		else if(*KeyInput == 's')// Replace with Down Arrow
		{
			if(cursor == GameStart)
			{
				cursor = Help;
				printf("Cursor is now at GameStart!\n");
			}
			else if(cursor == Help){}
			else
				cursor++; // Go Up if not Edge Case

		}

		else if(*KeyInput == ' ')
		{
			break;
		}


	}

}	


int main()
{
	gamemenu();
	printf("Cursor is at %i", cursor);
    return 0;
}

