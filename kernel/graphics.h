// Reasonably basic VGA-textmode graphics driver.

#ifndef KERNEL_GRAPHICS
#define KERNEL_GRAPHICS

#include <stddef.h>
#include <stdbool.h>

void graphics_init();
void kclear();											// Clears the screen to black.
void kcclear(char color);								// Clears the screen to specified color.
void kccolor(int pos, int n, char color);				// Changes a certain part of the screen's color.
void kscroll();											// Scrolls the screen one row either down, or up (up = true).
void kputchar(int pos, char c, char color);				// Puts the specified char at pos, with color.
void kputstring(int pos, char *text, int c, char color);// Put c characters of the specified string at pos with the color entered.
void kprint(int pos, char *text, char color);			// Prints a null terminated string at pos.
void kprintnum(int pos, int num, char color, int base); // Print an integer onto the screen at the specified position.

void kflip();											// Flip the back buffer onto video memory.

void set_vidptr(int pos);								// Set the vidptr to the position + framebuffer offset.
void handle_newline();									// Handle newline characters.

#endif