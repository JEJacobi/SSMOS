#ifndef KERNEL_GRAPHICS
#define KERNEL_GRAPHICS

#include <stddef.h>
#include <stdbool.h>

void graphics_init();
void clear();											// Clears the screen to black.
void cclear(char color);								// Clears the screen to specified color.
void ccolor(int pos, int n, char color);				// Changes a certain part of the screen's color.
void scroll();											// Scrolls the screen one row either down, or up (up = true).
void putchar(int pos, char c, char color);				// Puts the specified char at pos, with color.
void putstring(int pos, char *text, int c, char color);	// Put c characters of the specified string at pos with the color entered.
void print(int pos, char *text, char color);			// Prints a null terminated string at pos.
void printnum(int pos, int num, char color, int base); 	// Print an integer onto the screen at the specified position.

void flip();											// Flip the back buffer onto video memory.
void sync();											// Wait for vertical sync before proceeding.

static void set_vidptr(int pos);						// Set the vidptr to the position + framebuffer offset.
static void handle_newline();							// Handle newline characters.

#endif