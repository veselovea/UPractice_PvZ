
#include <iostream>
#include <string>
#include <vector>
#include "Utilities.h"
#include "Entities.h"

#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		printf(ESC "[2J") //lear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
#define visible_cursor() printf(ESC "[?251");
//Set Display Attribute Mode	<ESC>[{attr1};...;{attrn}m
#define resetcolor() printf(ESC "[0m")
#define set_display_atrib(color) 	printf(ESC "[%dm",color)

/*

--------------------------------------------------------------
	|P|P|									|			Z
--------------------------------------------------------------


*/
int _house_offset = 6;
int _game_zone_length;
int _zombie_place_offset = 20;
int _line_length;

int _rows, _columns, _lastLineY;

const char _ascii_uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int _ascii_uppercase_length = 27;

void writeColumns(int columns, int y) {
	gotoxy(_house_offset, y);
	for (int i = 0; i < columns; i++)
	{
		char sym = _ascii_uppercase[i];
		printf("|%c", sym);
		if (i + 1 == columns) {
			printf("|");
		}
	}
}

void DrawMap(int rows, int columns) {
	if (columns > _ascii_uppercase_length) {
		columns = _ascii_uppercase_length;
	}

	_rows = rows;
	_columns = columns;

	_game_zone_length = columns * 2;
	_line_length = _house_offset + _game_zone_length + _zombie_place_offset;

	writeColumns(columns, 1);

	int next_line_y = 2;
	std::string line(_line_length, '-');
	for (int i = 0; i < rows; i++)
	{
		if (i == 0) {
			gotoxy(1, next_line_y);
			std::cout << line;
		}

		gotoxy(2, next_line_y + 1);
		printf("%d", i + 1);

		gotoxy(_house_offset, next_line_y + 1);
		set_display_atrib(B_GREEN);
		printf("|");
		resetcolor();

		gotoxy(1, next_line_y + 2);
		std::cout << line;

		next_line_y += 2;
	}

	writeColumns(columns, next_line_y + 1);
	_lastLineY = next_line_y + 1;

	gotoxy(1, _lastLineY + 2);
}

void RefreshMap(std::vector<Entity*> entities) {
	clrscr();

	writeColumns(_columns, 1);
	std::string line(_line_length, '-');
	Home* home;

	for (auto& ent : entities)
	{
		gotoxy(ent->X, ent->Y);

		switch (ent->type)
		{
		case EntityType::HOME:
			home = dynamic_cast<Home*>(ent);

			set_display_atrib(B_GREEN);
			printf("|");
			resetcolor();

			gotoxy(1, ent->Y);
			std::cout << home->lineNumber;

			gotoxy(1, ent->Y - 1);
			std::cout << line;

			gotoxy(1, ent->Y + 1);
			std::cout << line;

			break;

		case EntityType::ZOMBIE:
			set_display_atrib(B_BLUE);
			printf("Z");
			resetcolor();
			break;

		case EntityType::PLANT:
			printf("1");
			break;
		}
	}

	writeColumns(_columns, _lastLineY);
	gotoxy(1, _lastLineY + 2);
}

int _Start() {
	/*gotoxy(4, 1);
	printf("|A|B|C|D|E\n");
	printf("------------------------------------------------------");

	gotoxy(1, 3);
	printf("1");

	gotoxy(4, 3);
	set_display_atrib(B_RED);
	printf("|");
	resetcolor();

	gotoxy(1, 4);
	printf("------------------------------------------------------");*/

	DrawMap(3, 10);

	return 1;
}


void test() {
	printf("1\n");
	printf("2\n");

	home();
	clrscr();
	printf("Home + clrscr\n");
	gotoxy(20, 7);
	printf("gotoxy(20,7)");

	gotoxy(1, 10);
	printf("gotoxy(1,10)  \n\n");

	set_display_atrib(BRIGHT);
	printf("Formatting text:\n");
	resetcolor();

	set_display_atrib(BRIGHT);
	printf("Bold\n");
	resetcolor();

	set_display_atrib(DIM);
	printf("Dim\n");
	resetcolor();

	set_display_atrib(BLINK);
	printf("Blink\n");
	resetcolor();

	set_display_atrib(REVERSE);
	printf("Reverse\n");
	printf("\n");


	set_display_atrib(BRIGHT);
	printf("Text color example:\n");
	resetcolor();

	set_display_atrib(F_RED);
	printf("Red\n");
	resetcolor();

	set_display_atrib(F_GREEN);
	printf("Green\n");
	resetcolor();

	set_display_atrib(F_BLUE);
	printf("Blue\n");
	resetcolor();

	set_display_atrib(F_CYAN);
	printf("Cyan\n");
	resetcolor();

	set_display_atrib(BRIGHT);
	printf("\nBottom color example:\n");
	resetcolor();

	set_display_atrib(B_RED);
	printf("Red\n");
	resetcolor();

	set_display_atrib(B_GREEN);
	printf("Green\n");
	resetcolor();

	set_display_atrib(B_BLUE);
	printf("Blue\n");
	resetcolor();

	set_display_atrib(B_CYAN);
	printf("Cyan\n");
	printf("\n");
	resetcolor();
}