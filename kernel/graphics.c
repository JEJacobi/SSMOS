#include "graphics.h"

#include <stddef.h>
#include <stdbool.h>
#include "math.h"
#include "memory.h"
#include "string.h"
#include "output.h"

#define FRAMEBUFFER_SIZE		0xFA0								// The size of the framebuffer (columns * rows * 2 bytes)
#define MEM_END					framebuffer + (COLUMNS * ROWS * 2) 	// The pointer to the end of video memory.
#define NUM_PRINT_SIZE			12									// The buffer size for printing numbers.

static volatile char *vidptr;
static char *numbuffer;
static void *framebuffer;
static void *memend;

void graphics_init()
{
	framebuffer = malloc(FRAMEBUFFER_SIZE);	//Get a chunk of memory to store the video framebuffer in.
	numbuffer = malloc(NUM_PRINT_SIZE);		//Get a chunk of memory to act as a buffer for printing numbers.
	vidptr = (volatile char*)framebuffer;
}

void clear()
{
	for( //Clear the video memory, color presets as well.
		vidptr = (volatile char*)framebuffer; 
		vidptr < MEM_END;
		vidptr++)
	{
		*vidptr = 0x0; //Just overwrite everything with NULs.
	}
}

void ccolor(int pos, int n, char color)
{
	int i;

	set_vidptr(pos);
	vidptr++; // Offset the video pointer to the color byte, instead of the character byte.
	
	for (i = 0; i < n; i++)
	{
		*vidptr = color; // Set the color.
		vidptr++;
		vidptr++; // Increment twice to the next color byte.
	}
}

void scroll()
{
	int i;
	
	// Start at row #2, go till ROWS - 1 (empty space for scrolling).
	for (i = 0; i < ROWS - 1; i++)
	{
		memcpy(
			(void*)((int)framebuffer + (get_position(0, i))), 	// The destination, row i-1.
			(void*)((int)framebuffer + (get_position(0, i + 1))), 		// The source, row i.
			COLUMNS * 2); // 2 bytes per entry, thus columns * 2 bytes.
	}
	
	memset( // Clear the last row of the framebuffer.
		(void*)((int)framebuffer + (get_position(0, ROWS - 1))),
		'\0',
		COLUMNS * 2);
}

void putchar(int pos, char c, char color)
{
	set_vidptr(pos);
	
	*vidptr = c; //Set the character.
	vidptr++; //Advance one byte.
	*vidptr = color; //Set the color.
}

void putstring(int pos, char *text, int c, char color)
{
	set_vidptr(pos);
	//TODO: Make sure nothing gets written outside video memory.
}

void print(int pos, char *text, char color)
{
	set_vidptr(pos);
	
	int i = 0; // Counter variable.
	
	while (text[i] != 0x0) // Write until a null terminator is reached.
	{
		//TODO: Check for vidptr exceeding video memory.
		
		if (text[i] != '\n') //if the character is not a newline
		{
			*vidptr = text[i]; // Write the character.
			vidptr++; // Increment to the color byte.
			*vidptr = color; // Write the color.
			vidptr++; // Increment for the next cycle.
		}
		else
		{
			handle_newline();
		}
		
		if (vidptr >= MEM_END)
			return; // If the vidptr is past the bounds of the video memory, stop printing and return.
			//TODO: Support scrolling?
		
		i++; // And increment the written counter.
	}
}

void printnum(int pos, int num, char color, int base)
{
	tostring(num, numbuffer, base); // Turn num into a string in specified base, store in numbuffer.
	print(pos, numbuffer, color); // And just print it as a string.
}

void flip()
{
	memcpy((void*)VIDEO_MEM,
			framebuffer,
			FRAMEBUFFER_SIZE); 
			//Copy the framebuffer to VIDEO_MEM, flipping the buffer onto active memory.
}

void sync()
{
	// Will need timers/IRQ? working.
}

static void set_vidptr(int pos)
{
	// Set the video pointer to the offset given, plus the generated framebuffer.
	vidptr = (volatile char*)(pos + framebuffer);
}

static void handle_newline()
{
	int i = 0;
	
	i = vidptr - (volatile char*)framebuffer; //Get the char's offset from a local reference point, instead of video memory.
	i = i % (COLUMNS * 2); //Find the amount needed to move the vidptr to a column boundary (new line).
	vidptr += (COLUMNS * 2) - i; //Increment vidptr by this amount.
}