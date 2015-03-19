#ifndef LIB_GRAPHICS
#define LIB_GRAPHICS
#include <math.h>

#define VIDEO_MEM	0xb8000	//Location of VGA video memory.
#define COLUMNS		80		//How many columns of characters.
#define ROWS		25		//How many rows of characters.

enum color					//VGA text mode color choices.
{
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GREY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	LIGHT_BROWN = 14,
	WHITE = 15,
};

inline int get_position(int x, int y)	//Get the combined position to write to.
{
	int val = 0;
	x = (int)clamp(x, 0, COLUMNS - 1);	//Clamp entries to existing columns and rows.
	y = (int)clamp(y, 0, ROWS - 1);
	
	val += x * 2;
	val += y * 160;
	return val;
}

inline char get_color(enum color fg, enum color bg)	//Get the formatted combined color for VGA text mode.
{
	return fg | bg << 4;
}

#endif