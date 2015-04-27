#include "graphics.h"

#include <stddef.h>
#include <stdbool.h>

#include "debug.h"
#include "list.h"
#include "math.h"
#include "memory.h"
#include "string.h"
#include "output.h"

#define FRAMEBUFFER_SIZE		(COLUMNS * ROWS * 2)			// The size of the framebuffer (columns * rows * 2 bytes)
#define MEM_END					framebuffer + FRAMEBUFFER_SIZE 	// The pointer to the end of video memory.

static volatile char *vidptr;
static char* numbuffer;
static void* framebuffer;

void graphics_init()
{
	framebuffer = kmalloc(FRAMEBUFFER_SIZE);	//Get a chunk of memory to store the video framebuffer in.
	numbuffer = kmalloc(MAX_INT_CHARS);		//Get a chunk of memory to act as a buffer for printing numbers.
	vidptr = (volatile char*)framebuffer;
	
	// Log the graphics initialization.
	string* logmsg = string_new();
	string_add(logmsg, "Graphics driver initialized, framebuffer allocated at 0x");
	string_addnum(logmsg, (int)framebuffer, 16);
	klog(logmsg);
}

void kclear()
{
	for( //Clear the video memory, color presets as well.
		vidptr = (volatile char*)framebuffer; 
		vidptr < (volatile char*)MEM_END;
		vidptr++)
	{
		*vidptr = 0x0; //Just overwrite everything with NULs.
	}
}

void kccolor(int pos, int n, char color)
{
	int i;

	set_vidptr(pos);
	vidptr++; // Offset the video pointer to the color byte, instead of the character byte.
	
	for (i = 0; i < n; i++)
	{
		*vidptr = color; // Set the color.
		vidptr++;
		vidptr++; // Increment twice to the next color byte.
		
		if (vidptr > (volatile char*)MEM_END)
			return;
	}
}

void kscroll()
{
	int i;
	
	// Start at row #2, go till ROWS - 1 (empty space for scrolling).
	for (i = 0; i < ROWS - 1; i++)
	{
		kmemcpy(
			(void*)((int)framebuffer + (get_position(0, i))), 	// The destination, row i-1.
			(void*)((int)framebuffer + (get_position(0, i + 1))), 		// The source, row i.
			COLUMNS * 2); // 2 bytes per entry, thus columns * 2 bytes.
	}
	
	kmemset( // Clear the last row of the framebuffer.
		(void*)((int)framebuffer + (get_position(0, ROWS - 1))),
		'\0',
		COLUMNS * 2);
}

void kputchar(int pos, char c, char color)
{
	set_vidptr(pos);
	
	*vidptr = c; //Set the character.
	vidptr++; //Advance one byte.
	*vidptr = color; //Set the color.
}

void kputstring(int pos, char *text, int c, char color)
{
	set_vidptr(pos);
	
	int i;
	
	for (i = 0; i < c; i++)
	{
		if (text[i] != '\n')
		{
			*vidptr = text[i]; 		// Write the character.
			vidptr++; 				// Increment to the color byte.
			*vidptr = color; 		// Write the color.
			vidptr++; 				// Increment for the next cycle.
		}
		else
		{
			handle_newline();
		}
	
		if (text[i] == 0x0)
			return; // If the string has run out before the characters, return.
	
		if (vidptr >= (volatile char*)MEM_END)
			return; // If the vidptr is past the bounds of the video memory, stop printing and return.
	}
}

void kprint(int pos, char *text, char color)
{
	set_vidptr(pos);
	
	int i = 0; // Counter variable.
	
	while (text[i] != 0x0) // Write until a null terminator is reached.
	{
		if (text[i] != '\n') //if the character is not a newline
		{
			*vidptr = text[i]; 		// Write the character.
			vidptr++; 				// Increment to the color byte.
			*vidptr = color; 		// Write the color.
			vidptr++; 				// Increment for the next cycle.
		}
		else
		{
			handle_newline();
		}
		
		if (vidptr >= (volatile char*)MEM_END)
			return; // If the vidptr is past the bounds of the video memory, stop printing and return.
		
		i++; // And increment the written counter.
	}
}

void kprintnum(int pos, int num, char color, int base)
{
	tostring(num, numbuffer, base); // Turn num into a string in specified base, store in numbuffer.
	kprint(pos, numbuffer, color); // And just print it as a string.
}

void kflip()
{
	kmemcpy((void*)VIDEO_MEM,
			framebuffer,
			FRAMEBUFFER_SIZE); 
			//Copy the framebuffer to VIDEO_MEM, flipping the buffer onto active memory.
}

void ksync()
{
	// Will need timers/IRQ? working.
}

void set_vidptr(int pos)
{
	// Set the video pointer to the offset given, plus the generated framebuffer.
	vidptr = (volatile char*)(pos + framebuffer);
}

void handle_newline()
{
	int i = 0;
	
	i = vidptr - (volatile char*)framebuffer; //Get the char's offset from a local reference point, instead of video memory.
	i = i % (COLUMNS * 2); //Find the amount needed to move the vidptr to a column boundary (new line).
	vidptr += (COLUMNS * 2) - i; //Increment vidptr by this amount.
}